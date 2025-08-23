
#include "structs.h"
#include "shm.h"

#define RESET   "\033[0m"
#define RED     "\033[41m"
#define GREEN   "\033[42m"
#define YELLOW  "\033[43m"
#define BLUE    "\033[44m"
#define MAGENTA "\033[45m"
#define CYAN    "\033[46m"

void printHeader(int columns) {
        int counter = 1;
        printf("╔");
        while (counter++ < columns) {
            printf("═══╦");
        }
        printf("═══╗\n");
}

void printTableContent(int height, int columns, GameState * game) {

    const char* colors[] = {RED, BLUE, GREEN, YELLOW, MAGENTA, CYAN};

    // Prueba 
    int testBoard[10][10];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            testBoard[i][j] = i;
        }
    }

    testBoard[0][0] = -1;
    testBoard[0][1] = -1;
    testBoard[1][2] = -1;
    testBoard[1][1] = -1;
    testBoard[5][5] = -2;
    testBoard[5][6] = -2;
    testBoard[5][7] = -2;
    testBoard[6][7] = -2;
    testBoard[7][7] = -2;
    testBoard[8][8] = -2;
    

    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            int val = testBoard[i][j];                          // Cambiar al tablero real, testBoard --> game->board[i * game->width + j]
            if (val < 0)
                printf("║%s%s %s", colors[-val - 1], ((i==1 && j==1) || (i==8 && j==8)) ? "🦆" : "  ",RESET);       // Hardcodeado para mostrar posicion actual
            else 
                printf("║ %d ", val); 
        }
        printf("║\n");
        if (i+1 < game->height) {
            printf("╠═══");
            for (int j = 1; j < game->width; j++) {
                printf("╬═══");
            }
            printf("╣\n");
        } 
    }
}

void printBase(int columns) {
    int counter = 1;
        printf("╚");
        while (counter++ < columns) {
            printf("═══╩");
        }
        printf("═══╝\n");
}


int main() {


    // conectar a mem compartida
    GameState * state = getGameState();
    GameSync * sync = getGameSync();
    

    while(!state->isGameOver)
    {
        // esperar aviso de master.c
        sem_wait(&sync->A);

    // TODO BORRAR, esto es para testear acceso a memoria compartida
        printf("Número de jugadores: %u\n", state->numPlayers);
        printf("Juego terminado: %d\n", state->isGameOver);
        printf("Dimensiones tablero: %u x %u\n", state->width, state->height);

        // este me sirve de debug de player.c
        printf("\n");
        printf("\n");
        printf("###  validMoves P1: %d    ###\n", state->players[0].validMoves);
        printf("###  invalidMoves P1: %d  ###\n", state->players[0].invalidMoves);
        printf("\n");

        
        printf("%p\n", (void *) state->board);
        printf("\n");


        printHeader(state->width);

        printTableContent(state->height, state->width, state);

        printBase(state->width);

        sem_post(&sync->B);
    }
    



    

    sem_post(&sync->B); // TODO este tal vez está de más

    

    // desconectar de mem compartida
    munmap(state, sizeof(GameState));
    munmap(sync, sizeof(GameSync));


    return 0;
    
}
