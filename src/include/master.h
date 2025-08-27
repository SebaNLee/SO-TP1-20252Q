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

GameState * initGameState(MasterParameters params);
GameSync * initGameSync();
int initSemaphores(GameSync * sync);
int freeSemaphores(GameSync * sync);

#endif