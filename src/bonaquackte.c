#include "quackenstein.h"
#include "structs.h"
#include "sync.h"
#include "math.h"


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

    while(!state->players[playerID].isBlocked)
    {
        moveProcessedWaitSync(sync, playerID);

        playerEntrySync(sync);
        
        // región crítica de lectura

      
        size_t stateSize = sizeof(GameState) + state->width * state->height * sizeof(int);
        GameState *localCopy = malloc(stateSize);

        if(localCopy == NULL)
        {
            perror("malloc error");
            exit(EXIT_FAILURE);
        }
        
        memcpy(localCopy, state, stateSize);
        
        playerExitSync(sync);

        unsigned char nextMove = computeNextMove(localCopy, state->players[playerID].x, state->players[playerID].y);
        free(localCopy);


        if (write(STDOUT_FILENO, &nextMove, sizeof(unsigned char)) == ERROR) {
            perror("Error in write");
            exit(EXIT_FAILURE);
        }
    }


    // desconectar de mem compartida
    closeSHM(state, stateSize);
    closeSHM(sync, syncSize);

    
    return 0;
}

bool isOutOfBoundaries(int x, int y, int height, int width) {
    return x < 0 || y < 0 || x >= width || y >= height;  
}

bool lastMove(int x, int y, GameState * state) {
    bool ans = true;
    for (int i = -1; i <= 1 && ans; i++) {
        for (int j = -1; j <= 1 && ans; j++) {
            if ( !isOutOfBoundaries(x+j, y+i, state->height, state->width) &&  state->board[(y+i) * state->width + (x+j)] > 0) 
                ans = false;
        }
    }
    return ans;
}

unsigned char planNextMove(int currentX, int currentY, GameState * state, int distance, bool askDir)  {
    int currentMax = 0;
    int worstCase = 0;
    unsigned char directions[][3] = { {7, 0, 1}, 
                                      {6, 9, 2},
                                      {5, 4, 3}};
    unsigned char maxDir;
    unsigned char worstDir;                                  
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int farPoints = 0;
            if (!isOutOfBoundaries(currentX+j, currentY+i, state->height, state->width) && state->board[(currentY+i) * state->width + (currentX + j)] > 0) {
                if (lastMove(currentX+j, currentY+i, state)) {
                    if ( worstCase < state->board[(currentY+i) * state->width + (currentX + j)] ) {
                        worstCase = state->board[(currentY+i) * state->width + (currentX + j)];
                        worstDir = directions[i+1][j+1];          
                    }
                } else {
                    if (distance > 1) {
                        farPoints = planNextMove(currentX+j, currentY+i, state, distance-1, false);
                    }
                    if (currentMax < state->board[(currentY+i) * state->width + currentX + j] + farPoints) { 
                        currentMax = state->board[(currentY+i) * state->width + currentX + j] + farPoints;
                        maxDir = directions[i+1][j+1];
                    }
                }
            }
        }
    }
    
    if (askDir) {
        return (currentMax > 0) ? maxDir : worstDir;
    }
    return (currentMax > 0) ? currentMax : worstCase;
    

}    

unsigned char computeNextMove(GameState *localCopy, int x, int y)
{
    if (lastMove(x, y, localCopy)) {
        return(EOF);
    }
    return(planNextMove(x, y, localCopy, 5, true));
}