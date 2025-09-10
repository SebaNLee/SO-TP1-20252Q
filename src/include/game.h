#ifndef GAME_H
#define GAME_H

#include "structs.h"
#include "parameters.h"
#include <math.h>
#include <sys/select.h>  
#include <time.h>

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1


typedef struct {
    unsigned char move;
    int playerIndex; 
} PlayerMove;


void setPlayerPosition(GameState * state, int width, int height, int numPlayers);
bool processMove(GameState * state, int i, char dirx, char diry);
PlayerMove waitPlayerMove(GameState * state, int pipesfd[][2], int timeout, time_t startTime, bool * isGameEnd);
void updateIfPlayerBlocked(GameState * state, int playerIndex);

#endif