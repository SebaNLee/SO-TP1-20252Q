#include "player.h"
#include "structs.h"
#include "sync.h"


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

    GameState * state = (GameState*) createSHM(GAME_STATE_SHM, stateSize, false, false);
    GameSync * sync = (GameSync*) createSHM(GAME_SYNC_SHM, syncSize, false, true);

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
        moveProcessedWaitSync(sync, playerID);

        playerEntrySync(sync);
        
        // región crítica de lectura

        // TODO extraer datos o copiar GameState
        
        playerExitSync(sync);

        unsigned char nextMove = computeNextMove();

        write(STDOUT_FILENO, &nextMove, sizeof(unsigned char)); 
    }


    // desconectar de mem compartida
    closeSHM(state, stateSize);
    closeSHM(sync, syncSize);

    
    return 0;
}




unsigned char computeNextMove()
{
    unsigned char moves[] = {0, 0, 2, 2, 4, 4, 6, 0, 1, 1, 1, 1, 1, 1};
    static int i = 0;

    return(moves[i++]);
}