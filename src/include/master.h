#ifndef MASTER_H
#define MASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "parameters.h"
#include "shm.h"
#include "structs.h"

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define ERROR -1
#define SUCCESS 0

void printParams(MasterParameters params); // TODO debug

void initGameState(GameState * state, MasterParameters parameters);
void initGameSync(GameSync * sync);
int initSemaphores(GameSync * sync);
int freeSemaphores(GameSync * sync);
void initPipes(int pipesfd[][2], int numPlayers);
void freePipes(int pipesfd[][2], int numPlayers);
void initPlayers(MasterParameters params, GameState * state, int pipesfd[][2]);
void initView(MasterParameters params);
void setPlayerPosition(GameState * state, int width, int height, int numPlayers);
void processMove(GameState * state, int i, char dirx, char diry);




#endif