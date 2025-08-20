
#include "structs.h"




int main() {


    // TODO BORRAR, esto es para testear acceso a memoria compartida



    // game_state
    int fd_state = shm_open("/game_state", O_RDONLY, 0);
    if (fd_state == -1) {
        perror("shm_open /game_state");
        exit(1);
    }

    GameState *state = mmap(NULL, sizeof(GameState), PROT_READ, MAP_SHARED, fd_state, 0);
    if (state == MAP_FAILED) {
        perror("mmap /game_state");
        exit(1);
    }

    

    printf("Número de jugadores: %u\n", state->numPlayers);
    printf("Juego terminado: %d\n", state->isGameOver);
    printf("Dimensiones tablero: %u x %u\n", state->width, state->height);





    // game_state
    int fd_sync = shm_open("/game_sync", O_RDONLY, 0);
    if (fd_sync == -1) {
        perror("shm_open /game_sync");
        exit(1);
    }

    GameSync *sync = mmap(NULL, sizeof(GameSync), PROT_READ, MAP_SHARED, fd_sync, 0);
    if (sync == MAP_FAILED) {
        perror("mmap /game_sync");
        exit(1);
    }






    // desconectar
    munmap(state, sizeof(GameState));
    munmap(sync, sizeof(GameSync));
    close(fd_state);
    close(fd_sync);


    return 0;
}
