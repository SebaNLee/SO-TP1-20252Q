#include "view.h"

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

void printTableContent(GameState * state) {

    const char* colors[] = {RED, BLUE, GREEN, YELLOW, MAGENTA, CYAN};

    int height = state->height;
    int width = state->width;
    int * board = state->board;
    
    printHeader(width);
    

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            
            int val = board[i * width + j];

            // TODO, acceder al array de Players para obtener la jugada actual (e imprimir como una cabeza) 🦆
            if(val <= 0)
            {
                printf("║%s   %s", colors[abs(val)], RESET); // TODO expandir vector de colores a jugadores máximos
            }
            else
            {
                printf("║ %d ", val);
            }

            
        }
        printf("║\n");
        if (i+1 < state->height) {
            printf("╠═══");
            for (int j = 1; j < width; j++) {
                printf("╬═══");
            }
            printf("╣\n");
        } 
    }

    printBase(width);
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
        printf("###  validMoves P1: %d    ###\n", state->players[0].validMoves);
        printf("###  invalidMoves P1: %d  ###\n", state->players[0].invalidMoves);
        printf("\n");

        printTableContent(state);

        sem_post(&sync->B);
    }
    

    sem_post(&sync->B); // TODO este tal vez está de más

    
    // desconectar de mem compartida
    munmap(state, sizeof(GameState));
    munmap(sync, sizeof(GameSync));


    return 0;
    
}
