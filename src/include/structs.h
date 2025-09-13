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

#define ERROR -1
#define SUCCESS 0

typedef struct
{
    char name[16];
    unsigned int score;
    unsigned int invalidMoves;
    unsigned int validMoves;
    unsigned short x, y;
    pid_t pid;
    bool isBlocked;
} Player;

typedef struct
{
    unsigned short width;
    unsigned short height;
    unsigned int numPlayers;
    Player players[9];
    bool isGameOver;
    int board[];
} GameState;

typedef struct
{

    sem_t view_reading_pending;
    sem_t view_writing_done;
    sem_t mutex_master_access;
    sem_t mutex_game_state_access;
    sem_t mutex_readers_counter;
    unsigned int readers_counter;
    sem_t move_processed[9];
} GameSync;

#endif