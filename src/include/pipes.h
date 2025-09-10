#ifndef PIPES_H
#define PIPES_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define PIPE_READ_END 0
#define PIPE_WRITE_END 1



void initPipes(int pipesfd[][2], int numPlayers);
void freePipes(int pipesfd[][2], int numPlayers);


#endif