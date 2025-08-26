#include "player.h"
#include "structs.h"


int main(int argc, char * argv[]) {

    // manejo de parámetros (asumo buen uso de master.c)
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    // TODO borrar dummy para que no me tire warning de unused variable
    int temp = width + height;
    temp++;
    
    // conectar a mem compartida
    GameState * state = getGameState();
    GameSync * sync = getGameSync();

    // obtengo número de jugador por PID guardado en struct Player
    int playerID;
    for(int i = 0; i < state->numPlayers; i++)
    {
        // si PID coincide con el i-ésimo jugador, entonces encontré playerID
        if(state->players[i].pid == getpid())
        {
            playerID = i;
        }
    }

    while(!state->isGameOver)
    {
        sem_wait(&sync->send_move[playerID]);
        
        int pipefd[2];

        if(pipe(pipefd) == -1)
        {
            perror("pipe error in player.c");
            exit(1);
        }

        // cierro el de read end del pipe, no me interesa desde player.c
        close(pipefd[PIPE_READ_END]);


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