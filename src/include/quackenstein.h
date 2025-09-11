#ifndef PLAYER_H
#define PLAYER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "structs.h"
#include "shm.h"

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1

unsigned char computeNextMove(GameState *localCopy, int x, int y);

#endif