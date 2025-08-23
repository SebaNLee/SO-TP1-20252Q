
#include "structs.h"
#include "shm.h"




int main() {


    // conectar a mem compartida
    GameState * state = getGameState();
    GameSync * sync = getGameSync();


    //sem_wait(&sync->A);



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
    
    
    for(int i = 0; i < state->height; i++)
    {
        for(int j = 0; j < state->width; j++)
        {
            printf(" %d ", state->board[i * state->width + j]);
        }

        printf("\n");
    }

    


    //sem_post(&sync->B);




    // desconectar de mem compartida
    munmap(state, sizeof(GameState));
    munmap(sync, sizeof(GameSync));


    return 0;
}
