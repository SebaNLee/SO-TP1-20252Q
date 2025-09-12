#ifndef SYNC_H
#define SYNC_H

#include "structs.h"

void masterEntrySync(GameSync * sync);
void masterExitSync(GameSync * sync);
void playerEntrySync(GameSync * sync);
void playerExitSync(GameSync * sync);
void moveProcessedPostSync(GameSync * sync, int i);
void moveProcessedWaitSync(GameSync * sync, int i);
void viewPrintSync(GameSync * sync);
void viewPostSync(GameSync * sync);
void viewWaitSync(GameSync * sync);
void moveProcessedPostAllSync(GameSync * sync, GameState * state);


#endif