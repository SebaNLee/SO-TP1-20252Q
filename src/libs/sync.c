#include "sync.h"
#include "structs.h"

void masterEntrySync(GameSync *sync)
{
    // bloqueo que entren lectores al agarrar este mutex
    if (sem_wait(&sync->mutex_master_access) == ERROR)
    {
        perror("Error in semwait");
        exit(EXIT_FAILURE);
    }

    // acceso exlusivo a GameState
    if (sem_wait(&sync->mutex_game_state_access) == ERROR)
    {
        perror("Error in semwait");
        exit(EXIT_FAILURE);
    }

    // libero mutex, lectores pueden entrar (igual van a quedar esperándose que suelte el mutex de GameState)
    if (sem_post(&sync->mutex_master_access) == ERROR)
    {
        perror("Error in sempost");
        exit(EXIT_FAILURE);
    }
}

void masterExitSync(GameSync *sync)
{
    // libero acceso exlusivo a GameState (ahora lectores van a leer)
    if (sem_post(&sync->mutex_game_state_access) == ERROR)
    {
        perror("Error in sempost");
        exit(EXIT_FAILURE);
    }
}

void playerEntrySync(GameSync *sync)
{
    // si master está queriendo acceder, player espera
    if (sem_wait(&sync->mutex_master_access) == ERROR)
    {
        perror("Error in semwait");
        exit(EXIT_FAILURE);
    }

    // si master no está acceso (o terminó de pedirlo), player continúa
    if (sem_post(&sync->mutex_master_access) == ERROR)
    {
        perror("Error in sempost");
        exit(EXIT_FAILURE);
    }

    // mutex para el contador
    if (sem_wait(&sync->mutex_readers_counter) == ERROR)
    {
        perror("Error in semwait");
        exit(EXIT_FAILURE);
    }

    // si soy el primero
    if (sync->readers_counter++ == 0)
    {
        // espero para agarrar el mutex de GameState
        if (sem_wait(&sync->mutex_game_state_access) == ERROR)
        {
            perror("Error in semwait");
            exit(EXIT_FAILURE);
        }
    }

    // mutex para el contador
    if (sem_post(&sync->mutex_readers_counter) == ERROR)
    {
        perror("Error in sempost");
        exit(EXIT_FAILURE);
    }
}

void playerExitSync(GameSync *sync)
{
    // mutex para el contador
    if (sem_wait(&sync->mutex_readers_counter) == ERROR)
    {
        perror("Error in semwait");
        exit(EXIT_FAILURE);
    }

    // si soy el último lector
    if (sync->readers_counter-- == 1)
    {
        // libero acceso exlusivo a GameState (ahora master puede acceder)
        if (sem_post(&sync->mutex_game_state_access) == ERROR)
        {
            perror("Error in sempost");
            exit(EXIT_FAILURE);
        }
    }

    // mutex para el contador
    if (sem_post(&sync->mutex_readers_counter) == ERROR)
    {
        perror("Error in sempost");
        exit(EXIT_FAILURE);
    }
}

void moveProcessedPostSync(GameSync *sync, int i)
{
    if (sem_post(&sync->move_processed[i]) == ERROR)
    {
        perror("Error in sempost");
        exit(EXIT_FAILURE);
    }
}

void moveProcessedWaitSync(GameSync *sync, int i)
{
    if (sem_wait(&sync->move_processed[i]) == ERROR)
    {
        perror("Error in semwait");
        exit(EXIT_FAILURE);
    }
}

void viewPrintSync(GameSync *sync)
{
    if (sem_post(&sync->view_reading_pending) == ERROR)
    {
        perror("Error in sempost");
        exit(EXIT_FAILURE);
    }
    if (sem_wait(&sync->view_writing_done) == ERROR)
    {
        perror("Error in semwait");
        exit(EXIT_FAILURE);
    }
}

void viewPostSync(GameSync *sync)
{
    if (sem_post(&sync->view_writing_done) == ERROR)
    {
        perror("Error in sempost");
        exit(EXIT_FAILURE);
    }
}

void viewWaitSync(GameSync *sync)
{
    if (sem_wait(&sync->view_reading_pending) == ERROR)
    {
        perror("Error in semwait");
        exit(EXIT_FAILURE);
    }
}

void moveProcessedPostAllSync(GameSync *sync, GameState *state)
{
    for (int i = 0; i < state->numPlayers; i++)
    {
        moveProcessedPostSync(sync, i);
    }
}