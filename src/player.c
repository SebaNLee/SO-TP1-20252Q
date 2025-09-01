#include "player.h"
#include "structs.h"


int main(int argc, char * argv[]) {

    // manejo de parámetros (asumo buen uso de master.c)
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    // conectar a mem compartida
    const char *GAME_STATE_SHM = "/game_state";
    const char *GAME_SYNC_SHM = "/game_sync";
    size_t boardSize = width * height * sizeof(int);
    size_t stateSize = sizeof(GameState) + boardSize;
    size_t syncSize = sizeof(GameSync);

    GameState * state = createSHM(GAME_STATE_SHM, stateSize, false, false);
    GameSync * sync = createSHM(GAME_SYNC_SHM, syncSize, false, true);

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
        close(pipefd[PIPE_READ_END]); // TODO !! mover a master.c


        // calculo siguiente jugada
        unsigned char nextMove = computeNextMove(); // TODO debería pasar parámetros como board, player, x, y, etc

        // consigna: "El máster garantiza que el extremo de escritura de este pipe esté asociado al descriptor de archivo 1"
        // escribo en write end del pipe (pasar siguiente jugada)
        write(STDOUT_FILENO, &nextMove, 1);

        
    }


    // desconectar de mem compartida
    closeSHM(state, stateSize);
    closeSHM(sync, syncSize);

    
    return 0;
}




unsigned char computeNextMove()
{
    srand(time(NULL));

    return rand() % 8;
}