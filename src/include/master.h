#ifndef MASTER_H
#define MASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parameters.h"
#include "structs.h"

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

void printParams(MasterParameters params); // TODO debug

GameState * initGameState();
GameSync * initGameSync();

#endif