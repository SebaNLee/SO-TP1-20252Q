#ifndef MASTER_H
#define MASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parameters.h"
#include "structs.h"

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1
#define ERROR -1
#define SUCCESS 0

void printParams(MasterParameters params); // TODO debug

GameState * initGameState();
GameSync * initGameSync();
int initSemaphores(GameSync * sync);
int freeSemaphores(GameSync * sync);

#endif