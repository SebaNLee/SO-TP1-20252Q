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

void *createSHM(const char *name, size_t size, bool create, bool write);
void closeSHM(void *ptr, int size);
void unlinkSHM(const char *name);

#endif