#include "sync.h"
#include "structs.h"


void masterEntrySync(GameSync * sync)
{
    sem_wait(&sync->mutex_master_access); // bloqueo que entren lectores al agarrar este mutex
    sem_wait(&sync->mutex_game_state_access); // acceso exlusivo a GameState
    sem_post(&sync->mutex_master_access); // libero mutex, lectores pueden entrar (igual van a quedar esperándose que suelte el mutex de GameState)
}


void masterExitSync(GameSync * sync)
{
    sem_post(&sync->mutex_game_state_access); // libero acceso exlusivo a GameState (ahora lectores van a leer)
}


void playerEntrySync(GameSync * sync)
{
    sem_wait(&sync->mutex_master_access); // si master está queriendo acceder, player espera
    sem_post(&sync->mutex_master_access); // si master no está acceso (o terminó de pedirlo), player continúa
    sem_wait(&sync->mutex_readers_counter); // mutex para el contador
    if(sync->readers_counter++ == 0) // si soy el primero
    {
        sem_wait(&sync->mutex_game_state_access); // espero para agarrar el mutex de GameState
    }
    sem_post(&sync->mutex_readers_counter); // mutex para el contador
}


void playerExitSync(GameSync * sync)
{
    sem_wait(&sync->mutex_readers_counter); // mutex para el contador
    if(sync->readers_counter-- == 1) // si soy el último lector
    {
        sem_post(&sync->mutex_game_state_access); // libero acceso exlusivo a GameState (ahora master puede acceder)
    }
    sem_post(&sync->mutex_readers_counter); // mutex para el contador
}

void moveProcessedPostSync(GameSync * sync, int i)
{
    sem_post(&sync->move_processed[i]);
}

void moveProcessedWaitSync(GameSync * sync, int i)
{
    sem_wait(&sync->move_processed[i]);
}

void viewPrintSync(GameSync * sync)
{
    sem_post(&sync->view_reading_pending);
    sem_wait(&sync->view_writing_done);
}

void viewPostSync(GameSync * sync)
{
    sem_post(&sync->view_writing_done);
}

void viewWaitSync(GameSync * sync)
{
    sem_wait(&sync->view_reading_pending);
}

void moveProcessedPostAllSync(GameSync * sync, GameState * state)
{
    for (int i = 0; i < state->numPlayers; i++) {
        moveProcessedPostSync(sync, i);
    }
}