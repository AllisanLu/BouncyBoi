#include "collision.h"

//1 = left bounce, 2 = middle bounce, 3 = right bounce, 0 = no bounce
int collision(int playerRow, int playerCol, int playerWidth, int bunnyRow, int bunnyCol, int bunnyWidth) {
    if (playerRow == bunnyRow + bunnyWidth 
        && bunnyCol < playerCol + 6
        && bunnyCol + bunnyWidth > playerCol) {
        return 1;
    } 
    else if (playerRow == bunnyRow + bunnyWidth 
        && bunnyCol < playerCol + playerWidth
        && bunnyCol >= playerCol + (playerWidth - 6)) {
        return 3;
    } 
    else if (playerRow == bunnyRow + bunnyWidth 
        && bunnyCol < playerCol + playerWidth
        && bunnyCol + bunnyWidth > playerCol) {
        return 2;
    } 
    return 0;
}

int overlap(int buffRow, int buffCol, int bunnyRow, int bunnyCol, int bunnyWidth) {
    if (buffRow > bunnyRow 
        && buffRow < bunnyRow + bunnyWidth
        && buffCol > bunnyCol
        && buffCol < bunnyCol + bunnyWidth) {
            return 1;
        }
    return 0;
}