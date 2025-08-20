

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>



typedef struct {
    char name[16];
    unsigned int score;
    unsigned int invalidMoves;
    unsigned int validMoves;
    unsigned short x, y;
    pid_t pid;
    bool isBlocked; // TODO puede estar invetido
} Player;

typedef struct {
    unsigned short width;
    unsigned short height;
    unsigned int numPlayers;
    Player players[9];
    bool isGameOver; // TODO puede estar invetido
    int * board;
} GameState;

typedef struct {
    sem_t A;
    sem_t B;
    sem_t C;
    sem_t D;
    sem_t E;
    unsigned int F;
    sem_t G[9];
} GameSync;




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
