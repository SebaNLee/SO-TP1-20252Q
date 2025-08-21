
// TODO mover a .h
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"


int main(){



    // TODO prueba de movimiento 
    // creo que en principio está jugando bien, pero no tengo forma de verificarlo
    // debe ser por semáforos (tira errores del tipo sem_)

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


    
    
    return 0;
}