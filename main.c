#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gba.h"
#include "movement.h"
#include "collision.h"

//images
#include "images/background1.h"
#include "images/background2.h"
#include "images/bouncyboi.h"
#include "images/verybouncyboi.h"
#include "images/gameover.h"
#include "images/platform.h"
#include "images/win.h"
#include "images/win1.h"
#include "images/moon.h"

enum gba_state {
  START,
  SETUP,
  INITCONTROLS,
  CONTROLS,
  PLAY,
  BOUNCING,
  WIN,
  LOSEINIT,
  LOSE,
};

int buttonWasDown[10] = {0};
int buttonJustReleased[10] = {0};
int score = 0;

int winningScore;

int main(void) {
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  int background = 0;
  int wincounter = 0;
  int collisionType = 0;

  // Load initial application state
  enum gba_state state = START;

  struct state cs;
  struct state ps;

  while (1) {
    ps = cs;
    currentButtons = BUTTONS; // Load the current state of the buttons

    	for (int i = 0; i < 10; i++) {
			  buttonJustReleased[i] = KEY_DOWN(i, BUTTONS) == 0 && buttonWasDown[i];
			  buttonWasDown[i] = KEY_DOWN(i, BUTTONS) != 0;
		  }

    //this is to set up for drawing
    switch(state) {
      case START:
        if (vBlankCounter %10 == 0) {
          if (!background) {
            background = 1;
          } else {
            background = 0;
          }
        }
      break;

      case SETUP:
        score = 0;
        cs.boi.row = cs.boi.size - 3;
        cs.boi.col = WIDTH/2;
        cs.boi.speedR = 1;
        cs.boi.speedC = 0;
        cs.boi.size = BOUNCYBOI_WIDTH;
        cs.boi.sprite = 0;

        cs.player.row = HEIGHT - 27;
        cs.player.col = WIDTH/2 - PLATFORM_WIDTH/2;
        cs.player.width = PLATFORM_WIDTH;
        cs.player.height = PLATFORM_HEIGHT;

        cs.buffAmount = 0;
        cs.currentBuffs = 0;
        cs.buffdurations[0] = 0;

        winningScore = randint(5, 12);
      break;

      case PLAY:
        //slow movespeed once buff is over
        for (int i = cs.currentBuffs - 1; i >= 0; i--) {
          if (cs.buffdurations[i] > 0) {
            cs.buffdurations[i]--;
          }
          if (cs.buffdurations[i] == 0) {
            if (cs.boi.speedR > 1 || cs.boi.speedR < -1) {
              cs.boi.speedR = cs.boi.speedR / 2;
            } 
            if (cs.boi.speedC > 1 || cs.boi.speedC < -1) {
              cs.boi.speedC = cs.boi.speedC / 2;
            }
            for (int j = i; j < cs.currentBuffs - 1; j++) {
              cs.buffdurations[j] = cs.buffdurations[j+1]; 
            }
            cs.currentBuffs--;
          }
        }

				if(moveBunny(&cs.boi.row, &cs.boi.speedR, &cs.boi.col, &cs.boi.speedC, cs.boi.size)) {
          state = LOSEINIT;
        }

        int speed = 2;
        if (KEY_DOWN(BUTTON_UP, currentButtons)) {
          movePlatform(&cs.player.row, -speed, &cs.player.col, 0, cs.player.width, cs.player.height);
			  }
			  if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
          movePlatform(&cs.player.row, speed, &cs.player.col, 0, cs.player.width, cs.player.height);
			  }
			  if(KEY_DOWN(BUTTON_LEFT, currentButtons)) {
          movePlatform(&cs.player.row, 0, &cs.player.col, -speed, cs.player.width, cs.player.height);
			  }
			  if(KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
				  movePlatform(&cs.player.row, 0, &cs.player.col, speed, cs.player.width, cs.player.height);
			  }

        collisionType = collision(cs.player.row, cs.player.col, cs.player.width, cs.boi.row, cs.boi.col, cs.boi.size);
        if (collisionType) {
          state = BOUNCING;
        }

        if (randint(0, 1000) > 990 && cs.buffAmount < MAX_BUFFS) {
          struct powerUp boost;
          boost.row = randint(10, HEIGHT/2);
          boost.col = randint(0, WIDTH);
          cs.buffs[cs.buffAmount] = boost;
          cs.buffAmount++;
        }

        for (int i = 0; i < cs.buffAmount; i++) {
          struct powerUp holder = cs.buffs[i];
          if (overlap(holder.row, holder.col, cs.boi.row, cs.boi.col, cs.boi.size)) {
            cs.boi.speedR = cs.boi.speedR * 2;
            cs.boi.speedC = cs.boi.speedC * 2;

            //add buff to slow down checker
            cs.buffdurations[cs.currentBuffs] = 35;
            cs.currentBuffs++;
            //remove
            for (int j = i; j < cs.buffAmount - 1; j++) {
              cs.buffs[j] = cs.buffs[j+1]; 
            }
            cs.buffAmount--;
          }
        }
        

      break;

      case BOUNCING:
        for (int i = cs.currentBuffs - 1; i >= 0; i--) {
          if (cs.buffdurations[i] > 0) {
            cs.buffdurations[i]--;
          }
          if (cs.buffdurations[i] == 0) {
            if (cs.boi.speedR > 1 || cs.boi.speedR < -1) {
              cs.boi.speedR = cs.boi.speedR / 2;
            } 
            if (cs.boi.speedC > 1 || cs.boi.speedC < -1) {
              cs.boi.speedC = cs.boi.speedC / 2;
            }
            for (int j = i; j < cs.currentBuffs - 1; j++) {
              cs.buffdurations[j] = cs.buffdurations[j+1]; 
            }
            cs.currentBuffs--;
          }
        }
        cs.boi.speedR = -cs.boi.speedR;
        if (collisionType == 1) {           
          if (cs.boi.speedC == 0) {
            cs.boi.speedC = cs.boi.speedR;
          }
          else if (cs.boi.speedC > 0) {
            cs.boi.speedC = -cs.boi.speedC;
          }
        } else if (collisionType == 2) {
          cs.boi.speedC = 0;
        } else if(collisionType == 3) {
          if (cs.boi.speedC == 0) {
            cs.boi.speedC = -cs.boi.speedR;
          }
          else if (cs.boi.speedC < 0) {
            cs.boi.speedC = -cs.boi.speedC;
          }
        }
        collisionType = 0;
        score++;
        if (score >= winningScore) {
          state = WIN;
        } else {
          state = PLAY;
        }
      break;

      case WIN:
        if (vBlankCounter % 10 == 0) {
          if (!wincounter) {
            wincounter = 1;
          } else {
            wincounter = 0;
          }
        }
      default: 
      break;  
    }

    waitForVBlank();
//this is the drawing and next state
    switch (state) {
      case START:
        if (background) {
          drawFullScreenImageDMA(background1);
        } else {
          drawFullScreenImageDMA(background2);
        } 

        if (buttonJustReleased[BUTTON_A]) {
          state = INITCONTROLS;
        }
        break;
      
      case INITCONTROLS:
        fillScreenDMA(BLACK);
        drawCenteredString(10, 0, WIDTH, 1, "CONTROLS", WHITE);
        drawString(20, 3, "A = continue", WHITE);
        drawString(30, 3, "right arrow = move platform right", WHITE);
        waitForVBlank();
        drawString(40, 3, "left arrow = move platform left", WHITE);
        drawString(50, 3, "up arrow = move platform up", WHITE);
        drawString(60, 3, "down arrow = move platform down", WHITE);
        drawString(70, 3, "select = go back to title screen", WHITE);

        drawString(90, 3, "Goal of the game:", WHITE);
        drawString(100, 3, "Don't let the rabbit fall!", WHITE);
        drawString(110, 3, "For how long? Who knows?", WHITE);
        state = CONTROLS;
      break;

      case CONTROLS:
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
        }
        if(buttonJustReleased[BUTTON_A]) {
          state = SETUP;
        }
      break;

      case SETUP:
        fillScreenDMA(BLACK);
        char *status = "Score: ";
        char num[12];
        sprintf(num, "%d", score);
        drawString(10, 42, num, WHITE);
        drawString(10, 5, status, WHITE);

        state = PLAY;
      break;

      case PLAY:
      //clear
        drawRectDMA(ps.boi.row, ps.boi.col, ps.boi.size, ps.boi.size, BLACK);
        drawRectDMA(10, 42, 118, 10, BLACK);
        drawRectDMA(ps.player.row, ps.player.col, ps.player.width, ps.player.height, BLACK);
      //draw
        for (int i = 0; i < cs.buffAmount; i++) {
          struct powerUp holder = cs.buffs[i];
          drawRectDMA(holder.row, holder.col, 1, 1, WHITE);
        }
        drawImageDMA(HEIGHT - MOON_HEIGHT, 0, MOON_WIDTH, MOON_HEIGHT, moon);
        drawImageDMA(cs.boi.row, cs.boi.col, cs.boi.size, cs.boi.size, bouncyboi);
        drawString(10, 5, status, WHITE);
        sprintf(num, "%d", score);
        drawString(10, 42, num, WHITE);
        drawImageDMA(cs.player.row, cs.player.col, cs.player.width, cs.player.height, platform);
        if (KEY_DOWN(BUTTON_SELECT, currentButtons)) {
          state = START;
        }
      break;

      case BOUNCING:
      //clear
        drawRectDMA(ps.player.row, ps.player.col, ps.player.width, ps.player.height, BLACK);
        drawRectDMA(ps.boi.row, ps.boi.col, ps.boi.size, ps.boi.size, BLACK);
        drawRectDMA(10, 42, 118, 10, BLACK);

      //draw          
        drawImageDMA(HEIGHT - MOON_HEIGHT, 0, MOON_WIDTH, MOON_HEIGHT, moon);
        drawImageDMA(cs.player.row, cs.player.col, cs.player.width, cs.player.height, platform);
        drawImageDMA(cs.boi.row, cs.boi.col, cs.boi.size, cs.boi.size, verybouncyboi);
      
        sprintf(num, "%d", score);
        drawString(10, 42, num, WHITE);
        drawString(10, 5, status, WHITE);
        for (int i = 0; i < 30; i++) {
          waitForVBlank();
        }
      break;

      case WIN:
        if (wincounter) {
          drawFullScreenImageDMA(win1);
        } else {
          drawFullScreenImageDMA(win);
        } 

        if (KEY_DOWN(BUTTON_SELECT, currentButtons)){
          state = START;
        }
      break;

      case LOSEINIT:
        drawFullScreenImageDMA(gameover);
        waitForVBlank();
        drawCenteredString(0, 0, WIDTH, 20, "Game Over", WHITE);
        drawString(20, 2, "Press select to go back to title screen", WHITE);
        state = LOSE;
      break;

      case LOSE:
        if (buttonJustReleased[BUTTON_SELECT]){
          state = START;
        }
        break;
    }
    previousButtons = currentButtons; // Store the current state of the buttons
  }

  UNUSED(previousButtons); // You can remove this once previousButtons is used

  return 0;
}
