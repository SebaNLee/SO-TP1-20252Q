#ifndef GAME_H
#define GAME_H

#include "structs.h"
#include <math.h>


void setPlayerPosition(GameState * state, int width, int height, int numPlayers);
bool processMove(GameState * state, int i, char dirx, char diry);

#endif