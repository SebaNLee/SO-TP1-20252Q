#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "structs.h"
#include "shm.h"
#include "sync.h"
#include "math.h"

unsigned char computeNextMove()
{
    srand(time(NULL));

    return rand() % 8;
}

int main(int argc, char *argv[])
{
    // manejo de parámetros (asumo buen uso de master.c)
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    // conectar a mem compartida
    const char *GAME_STATE_SHM = "/game_state";
    const char *GAME_SYNC_SHM = "/game_sync";
    size_t boardSize = width * height * sizeof(int);
    size_t stateSize = sizeof(GameState) + boardSize;
    size_t syncSize = sizeof(GameSync);

    GameState *state = (GameState *)createSHM(GAME_STATE_SHM, stateSize, false, false);
    GameSync *sync = (GameSync *)createSHM(GAME_SYNC_SHM, syncSize, false, true);

    // obtengo número de jugador por PID guardado en struct Player
    int playerID;
    for (int i = 0; i < state->numPlayers; i++)
    {
        // si PID coincide con el i-ésimo jugador, entonces encontré playerID
        if (state->players[i].pid == getpid())
        {
            playerID = i;
        }
    }

    while (!state->players[playerID].isBlocked)
    {
        moveProcessedWaitSync(sync, playerID);

        playerEntrySync(sync);

        // región crítica de lectura

        size_t stateSize = sizeof(GameState) + state->width * state->height * sizeof(int);
        GameState *localCopy = malloc(stateSize);

        if (localCopy == NULL)
        {
            perror("malloc error");
            exit(EXIT_FAILURE);
        }

        memcpy(localCopy, state, stateSize);

        playerExitSync(sync);

        unsigned char nextMove = computeNextMove();
        free(localCopy);

        if (write(STDOUT_FILENO, &nextMove, sizeof(unsigned char)) == ERROR)
        {
            perror("Error in write");
            exit(EXIT_FAILURE);
        }
    }

    // desconectar de mem compartida
    closeSHM(state, stateSize);
    closeSHM(sync, syncSize);

    return 0;
}
