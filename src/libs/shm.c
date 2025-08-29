#include "shm.h"




// TODO tal vez se tenga que hacer con permisos de read only o con write tmb

GameState * getGameState()
{
    int fd_state = shm_open("/game_state", O_RDONLY, 0); // TODO ojo que esto está en RDONLY
    if (fd_state == -1)
    {
        perror("shm_open /game_state");
        exit(1);
    }

    GameState *state = mmap(NULL, sizeof(GameState), PROT_READ, MAP_SHARED, fd_state, 0); // TODO ojo que esto está en RDONLY
    if (state == MAP_FAILED)
    {
        perror("mmap /game_state");
        exit(1);
    }

    close(fd_state);

    return state;
}


GameSync * getGameSync()
{
    int fd_sync = shm_open("/game_sync", O_RDWR, 0);
    if (fd_sync == -1)
    {
        perror("shm_open /game_sync");
        exit(1);
    }

    GameSync *sync = mmap(NULL, sizeof(GameSync), PROT_READ | PROT_WRITE, MAP_SHARED, fd_sync, 0);
    if (sync == MAP_FAILED)
    {
        perror("mmap /game_sync");
        exit(1);
    }

    close(fd_sync);

    return sync;
}

void closeGameState(GameSync * sync)
{
    munmap(sync, sizeof(GameSync));

    return;
}


void closeGameSync(GameSync * state)
{
    munmap(state, sizeof(GameState));
    
    return;
}

void * createSHM(const char *name, size_t size) {

    // Creación de memoria
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Failed to create shared memory");
        exit(1);
    }

    // Ajuste de tamaño
    if (ftruncate(fd, size) == -1) {
        perror("Failed to truncate shared memory");
        exit(1);
    }

    // Mapeo
    void * ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Failed to map memory");
        exit(1);
    }

    // Ya no lo necesitamos
    close(fd);
    return ptr;

}


void closeSHM(void * ptr, int size)
{
    if(munmap(ptr, size) == -1)
    {
        perror("munmap() error\n");
        exit(1);
    }
}


void unlinkSHM(const char * name)
{
    if(shm_unlink(name) == -1)
    {
        perror("shm_unlink() error\n");
        exit(1);
    }

    return;
}


    
    
    
    