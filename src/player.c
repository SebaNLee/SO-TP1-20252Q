
// TODO mover a .h
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"
#include "shm.h"


int main(){



    // TODO prueba de movimiento 
    // creo que en principio está jugando bien, pero no tengo forma de verificarlo
    // debe ser por semáforos (tira errores del tipo sem_)


    // conectar a mem compartida
    GameState * state = getGameState();
    GameSync * sync = getGameSync();

    while(!state->isGameOver)
    {

        sem_wait(&sync->G[0]); // TODO hardcodeado con 0, cambiar

        
        int pipefd[2];

        if(pipe(pipefd) == -1)
        {
            perror("pipe error in player.c");
            exit(1);
        }

        // cierro el de read end del pipe, no me interesa desde player.c
        close(pipefd[0]);


        // preparo para escribir siguiente jugada
        // consigna: "El máster garantiza que el extremo de escritura de este pipe esté asociado al descriptor de archivo 1"
        unsigned char nextMove = 4; // TODO meter random después

        // escribo en write end del pipe (pasar siguiente jugada)
        write(STDOUT_FILENO, &nextMove, 1);

        sem_post(&sync->G[0]); // TODO hardcodeado con 0, cambiar
    }


    // desconectar de mem compartida
    munmap(state, sizeof(GameState));
    munmap(sync, sizeof(GameSync));

    
    return 0;
}