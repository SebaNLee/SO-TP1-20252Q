#ifndef SHM_H
#define SHM_H

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "structs.h"



GameState * getGameState();
GameSync * getGameSync();
void closeGameState();
void closeGameSync();
void * createSHM(const char *name, size_t size);




#endif