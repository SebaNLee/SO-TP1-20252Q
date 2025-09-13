#ifndef VIEW_H
#define VIEW_H

#include "structs.h"
#include "shm.h"
#include <math.h>
#include <string.h>

void tabulate(int width, const char *color);
void printTitle(int width);
void newLine();
void printHeader(int columns, int rows);
void printTableContent(GameState *state, int width, int height, Player **leaderboard);
void printBase(int columns);
bool isDraw(Player *p1, Player *p2);

#endif