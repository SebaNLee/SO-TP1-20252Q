#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdio.h>

#define MINPLAYERS 1
#define MAXPLAYERS 9

typedef struct
{
    int width;
    int height;
    int delay;
    int timeout;
    long seed;
    char *view;
    int numPlayers;
    char *players[MAXPLAYERS];

} MasterParameters;

MasterParameters setParams(int argc, char *const argv[]);
void printParams(MasterParameters params);

#endif