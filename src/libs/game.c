#include "game.h"

int rowMov[DIRECTION_OPTIONS] = {-1, -1, 0, 1, 1, 1, 0, -1};
int columnMov[DIRECTION_OPTIONS] = {0, 1, 1, 1, 0, -1, -1, -1}; 


//separa el tablero por regiones y le asigna el centro de cada region a un jugador
void setPlayerPosition(GameState * state, int width, int height, int numPlayers){
  
    int rows, cols;


    rows = cols = (int)ceil(sqrt(numPlayers));

    int idx = 0;
    for (int r = 0; r < rows && idx < numPlayers; r++) {
        for (int c = 0; c < cols && idx < numPlayers; c++) {
            
            int x = (c * width / cols) + (width / (2 * cols));
            int y = (r * height / rows) + (height / (2 * rows));

            state->players[idx].x = x;
            state->players[idx].y = y;

            // marcar celda como tomada por este jugador
            state->board[y * width + x] = -(idx); 

            idx++;
        }
    }

}


// devuelve true si es jugada válida, false si no es inválida
bool processMove(GameState * state, int i, unsigned char move) {

    char diry = rowMov[move];
    char dirx = columnMov[move];

    int finalXpos = state->players[i].x + dirx;
    int finalYpos = state->players[i].y + diry;
    if ( finalXpos < 0 || finalXpos >= state->width || finalYpos < 0 || finalYpos >= state->height || state->board[finalXpos + state->width * finalYpos] <= 0) {
        state->players[i].invalidMoves++;
        return false;
    } else {
        state->players[i].score += state->board[finalXpos + state->width * finalYpos];
        state->players[i].validMoves++;
        state->board[finalXpos + state->width * finalYpos] = -i;
        state->players[i].x = finalXpos;
        state->players[i].y = finalYpos;

        return true;
    }

}


PlayerMove waitPlayerMove(GameState * state, int pipesfd[][2], int timeout, time_t startTime, bool * isGameEnd)
{
    PlayerMove toReturn = {.playerIndex = -1, .move = EOF};
    static int lastProcessedPlayer = 0;

    // Crear el set de pipes que se leen en select
    fd_set fds;
    FD_ZERO(&fds);

    // Cálculo de maxfd
    int maxfd = 0;
    for (int i = 0; i < state->numPlayers; i++) {
        if (pipesfd[i][PIPE_READ_END] > maxfd)
            maxfd = pipesfd[i][PIPE_READ_END];
    }

    // meto al set los pipes de los jugadores activos
    for (int i = 0; i < state->numPlayers; i++) {
        if (!state->players[i].isBlocked)
            FD_SET(pipesfd[i][PIPE_READ_END], &fds);
    }

    // Seteo de timeout base
    int elapsed = (int)(time(NULL) - startTime);
    int remaining = timeout - elapsed;
    if (remaining < 0)
    {
        remaining = 0;
    }
    struct timeval timeInterval = {.tv_sec = remaining, .tv_usec = 0}; // TODO cálculo de milisegundos?

    // chequear si algún jugador mandó movimiento
    int activity = select(maxfd + 1, &fds, NULL, NULL, &timeInterval);

    if (activity < 0)
    {
        *isGameEnd = true;
    }
    // no hubo writes de jugadores en tiempo timeout (entonces salgo)
    else if(activity == 0)
    {
        *isGameEnd = true;
    }
    // si hubo devoluciones, agarro con round robin al primer fd con datos
    else if(activity > 0)
    {
        int start = (lastProcessedPlayer++) % state->numPlayers;

        for (int offset = 0; offset < state->numPlayers; offset++) {
            
            int i = (start + offset) % state->numPlayers;

            if (!state->players[i].isBlocked && FD_ISSET(pipesfd[i][PIPE_READ_END], &fds)) {
                
                unsigned char move;
                read(pipesfd[i][PIPE_READ_END], &move, sizeof(move));

                toReturn.playerIndex = i;
                toReturn.move = move;

                break;
            }
        }
    }

    return toReturn;
}