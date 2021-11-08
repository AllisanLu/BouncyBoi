#include "movement.h"
#include "gba.h"

//returns true if bunny hits the ground
int moveBunny(int *row, int *speedR, int *col, int *speedC, int size) {
    *row = *row + *speedR;
	*col += *speedC;
	if(*row < 0) {
        *row = 0;
		*speedR = -*speedR;
        int leftOrRight = randint(-1, 2);
        *speedC = *speedR * leftOrRight;
	}
	if(*row > HEIGHT-size) {
        *row = HEIGHT-size;
		*speedR = 0;
        *speedC = 0;
        return 1;
	}
	if(*col < 0) {
		*col = 0;
		*speedC = -*speedC;
	}
	if(*col > WIDTH-size) {
		*col = WIDTH - size;
		*speedC = -*speedC;
	}
	return 0;
}

//returns true if there is a collision with player
void movePlatform(int *row, int speedR, int *col, int speedC, int width, int height) {
	*row = *row + speedR;
	*col = *col + speedC;

	if(*row < 0) {
        *row = 0;
	}
	if(*row < (HEIGHT/2) - height) {
        *row = (HEIGHT/2)-height;
	}
	if(*col < 0) {
		*col = 0;
	}
	if(*col > WIDTH-width) {
		*col = WIDTH - width;
	}
}
