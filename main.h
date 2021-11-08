#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

// TODO: Create any necessary structs

/*
* For example, for a Snake game, one could be:
*
* struct snake {
*   int heading;
*   int length;
*   int row;
*   int col;
* };
*
* Example of a struct to hold state machine data:
*
* struct state {
*   int currentState;
*   int nextState;
* };
*
*/

#define MAX_BUFFS (5)

struct boi {
    int row;
    int col;
    int sprite;
    int size;
    int speedR;
    int speedC;
};

struct platform {
    int row;
    int col;
    int width;
    int height;
};

struct powerUp {
    int row;
    int col;
    int size;
};

struct state {
    int currentState;
    int nextState;
    struct boi boi;
    struct platform player;
    int buffAmount;
    struct powerUp buffs[MAX_BUFFS];
    int buffdurations[MAX_BUFFS];
    int currentBuffs;
};
#endif
