#ifndef INIT_H
#define INIT_H

#include "structs.h"
#include "parameters.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>



#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

// TODO, habría que ver si los de state y sync van en shm.c o no

void initGameState(GameState * state, MasterParameters parameters);
void initGameSync(GameSync * sync);
int freeGameSync(GameSync * sync);
void initPlayers(MasterParameters params, GameState * state, int pipesfd[][2]);
int initView(MasterParameters params);
void initPipes(int pipesfd[][2], int numPlayers);
void freePipes(int pipesfd[][2], int numPlayers);
void waitViewAndPlayers(GameState * state, int viewPID);

#endif