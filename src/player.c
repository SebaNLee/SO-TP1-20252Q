#include "player.h"


int main(){


    // conectar a mem compartida
    GameState * state = getGameState();
    GameSync * sync = getGameSync();

    // meto random 

    while(!state->isGameOver)
    {
        // TODO
        // esto anda para un jugador con id 0
        // es una prueba genérica


        sem_wait(&sync->G[0]); // TODO hardcodeado con 0, cambiar

        
        int pipefd[2];

        if(pipe(pipefd) == -1)
        {
            perror("pipe error in player.c");
            exit(1);
        }

        // cierro el de read end del pipe, no me interesa desde player.c
        close(pipefd[0]);


        // calculo siguiente jugada
        unsigned char nextMove = computeNextMove(); // TODO debería pasar parámetros como board, player, x, y, etc

        // consigna: "El máster garantiza que el extremo de escritura de este pipe esté asociado al descriptor de archivo 1"
        // escribo en write end del pipe (pasar siguiente jugada)
        write(STDOUT_FILENO, &nextMove, 1);

        
    }


    // desconectar de mem compartida
    munmap(state, sizeof(GameState));
    munmap(sync, sizeof(GameSync));

    
    return 0;
}




unsigned char computeNextMove()
{
    srand(time(NULL));

    return rand() % 8;
}