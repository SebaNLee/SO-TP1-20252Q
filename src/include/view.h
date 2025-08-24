#ifndef VIEW_H
#define VIEW_H

#include "structs.h"
#include "shm.h"


void printHeader(int columns);

void printTableContent(GameState * state, int width, int height);

void printBase(int columns);



#endif