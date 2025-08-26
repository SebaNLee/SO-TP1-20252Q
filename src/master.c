#include "master.h"
#include "structs.h"








int main(int argc, char const *argv[]) {


    MasterParameters params = setParams(argc, ( char * const *) argv);


    GameState * state = initGameState();

    GameSync * sync = initGameSync();

    
    // incializo pipes para cada jugador
    //int pipesfd[numPlayers][2];


    printParams(params);


}

GameState * initGameState()
{
    // TODO aca iniciar y devolver shmem

    
    // TODO debug
    return NULL;
}

GameSync * initGameSync()
{
    // TODO aca iniciar y devolver shmem


    // TODO debug
    return NULL;
}



// TODO debug
//solo de testeo
void printParams(MasterParameters params){
    printf("width=%d\theight=%d\tdelay=%d\ttimeout=%d\tseed=%ld\tview=%s\tnumPlayers=%d\n", params.width, params.height, params.delay, params.timeout, params.seed, params.view, params.numPlayers);
    for(int i=0; i<params.numPlayers; i++){
        printf("player %d 's name=%s\n", i+1, params.players[i]);
    }
    
}