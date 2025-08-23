#include "shm.h"



GameState * getGameState()
{
    int fd_state = shm_open("/game_state", O_RDONLY, 0);
    if (fd_state == -1)
    {
        perror("shm_open /game_state");
        exit(1);
    }

    GameState *state = mmap(NULL, sizeof(GameState), PROT_READ, MAP_SHARED, fd_state, 0);
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
    int fd_sync = shm_open("/game_sync", O_RDONLY, 0);
    if (fd_sync == -1)
    {
        perror("shm_open /game_sync");
        exit(1);
    }

    GameSync *sync = mmap(NULL, sizeof(GameSync), PROT_READ, MAP_SHARED, fd_sync, 0);
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



    
    
    
    