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
    int board[];
} GameState;

typedef struct {
    /*
    El máster hace sem_post(&A) cuando cambió el estado (ej: después de procesar un movimiento)
    La vista hace sem_wait(&A) para enterarse que hay algo nuevo que imprimir
    */
    sem_t view_reading_pending; 
     /*
     La vista hace sem_post(&B) cuando terminó de imprimir
    El máster hace sem_wait(&B) para esperar antes de continuar
     */
    sem_t view_writing_done; 

    sem_t mutex_master_entry; // indica que el master quiere acceder exclusivamente a GameState
    sem_t mutex_game_state; // acceso exclusivo para GameState
    sem_t mutex_counter; //se usa para evitar que lectores entren mientras el escritor espera
    unsigned int numReaders;//contador de cuántos jugadores están leyendo.
    sem_t send_move[9]; // Le indican a cada jugador que puede enviar 1 movimiento
} GameSync;

/*
A y B → Comunicación Master ↔ Vista

A (view_reading_pending):
El máster hace sem_post(&A) cuando cambió el estado (ej: después de procesar un movimiento).
La vista hace sem_wait(&A) para enterarse que hay algo nuevo que imprimir.

B (view_writing_done):
La vista hace sem_post(&B) cuando terminó de imprimir.
El máster hace sem_wait(&B) para esperar antes de continuar.
Esto asegura que no se pisa la memoria mientras la vista imprime.

C,D,E,F (JUGAODRES<->MASTER)
Estos cuatro implementan el clásico problema de lectores/escritores:
**Los jugadores son lectores (leen el estado para decidir un movimiento).
**El máster es el único escritor (modifica el tablero y los puntajes).

Objetivo: que muchos jugadores puedan leer en paralelo, pero que cuando el máster quiera escribir:
**espere a que terminen todos los lectores actuales,
**y bloquee a los nuevos lectores hasta que termine.

Evitamos condiciones de carrera y la inanición del escritor.
**C (mutex_readers): protege el acceso al contador F (para sumar/restar lectores).
**D (mutex_writers): se bloquea cuando alguien escribe (el máster).
**E (mutex_counter): se usa para evitar que lectores entren mientras el escritor espera.
**F (numReaders): contador de cuántos jugadores están leyendo.

Ejemplo flujo:

Jugador entra a leer:

sem_wait(C) → modifica F++.

Si es el primer lector, hace sem_wait(D) (bloquea al escritor).

sem_post(C).

Jugador termina de leer:

sem_wait(C) → modifica F--.

Si es el último lector, hace sem_post(D) (libera al escritor).

sem_post(C).

Máster quiere escribir:

sem_wait(D) → espera a que no haya lectores.

Modifica estado.

sem_post(D) → libera acceso.

*/

#endif