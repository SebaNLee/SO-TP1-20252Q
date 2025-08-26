#ifndef MASTER_H
#define MASTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parameters.h"
#include "structs.h"

void printParams(MasterParameters params); // TODO debug

GameState * initGameState();
GameSync * initGameSync();

#endif