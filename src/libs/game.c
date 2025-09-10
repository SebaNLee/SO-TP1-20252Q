#include "game.h"


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
bool processMove(GameState * state, int i, char dirx, char diry) {

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