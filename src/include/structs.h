#ifndef STRUCTS_H
#define STRUCTS_H

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
// TODO sacar lo que no se use

typedef struct {
    char name[16];
    unsigned int score;
    unsigned int invalidMoves;
    unsigned int validMoves;
    unsigned short x, y;
    pid_t pid;
    bool isBlocked; // TODO puede estar invetido
} Player;

typedef struct {
    unsigned short width;
    unsigned short height;
    unsigned int numPlayers;
    Player players[9];
    bool isGameOver; // TODO puede estar invetido
    int * board;
} GameState;

typedef struct {
    sem_t A;
    sem_t B;
    sem_t C;
    sem_t D;
    sem_t E;
    unsigned int F;
    sem_t G[9];
} GameSync;


#endif