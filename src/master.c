
#include "master.h"
#include <sys/select.h>   
#include "structs.h"
#include <math.h>
#include "sync.h"
#include "game.h"
#include "init.h"

#define DIRECTION_OPTIONS 8
int rowMov[DIRECTION_OPTIONS] = {-1, -1, 0, 1, 1, 1, 0, -1};
int columnMov[DIRECTION_OPTIONS] = {0, 1, 1, 1, 0, -1, -1, -1}; 



int main(int argc, char const *argv[]) {

    const char *GAME_STATE_SHM = "/game_state";
    const char *GAME_SYNC_SHM = "/game_sync";

    // parseo de parámetros
    MasterParameters params = setParams(argc, ( char * const *) argv);

    // TODO debug
    printParams(params);

    GameState * state = (GameState*) createSHM(GAME_STATE_SHM, sizeof(GameState) + params.width * params.height * sizeof(int), true, true);
    GameSync * sync = (GameSync *) createSHM(GAME_SYNC_SHM, sizeof(GameSync), true, true);

    initGameState(state, params);
    initGameSync(sync);
    
    // incializo pipes para cada jugador
    int pipesfd[state->numPlayers][2];
    initPipes(pipesfd, state->numPlayers);

    // inicializo jugadores y vista
    initPlayers(params, state, pipesfd);
    initView(params);
    setPlayerPosition(state, state->width, state->height, state->numPlayers);


    // es por si otro procesos como view del master consumen demasiado tiempo
    // observar que al hacer strace, ChomChamps hace a veces 10s o 9s con -t 10
    time_t startTime;
    
    // para terminar el juego si no hubo movimientos válidos en timeout tiempo
    time_t lastValidMoveTime = time(NULL);
    bool validMove;

    // en clase se dijo que se debían inicializar en 1, todos pueden mover
    // TODO moverlo a init que está feo acá
    for (int i = 0 ; i < state->numPlayers; i++) { 
        sem_post(&sync->move_processed[i]); 
    }

    while(!state->isGameOver)
    {
        startTime = time(NULL);
        validMove = false;

        // indico a view que imprima y espero a que termine
        viewPrintSync(sync);

        // consigna, delay después de imprimir con view
        usleep(params.delay * 1000);

        // espero jugada
        bool isGameEnd = false;
        PlayerMove playerMove = waitPlayerMove(state, pipesfd, params.timeout, startTime, &isGameEnd);

        if(isGameEnd)
        {
            
            state->isGameOver = true;
            break;
        }

        // TODO chequear EOF en consigna
        if(playerMove.move == EOF)
        {
            state->players[playerMove.playerIndex].isBlocked = 1;
        }
        else
        {
            char diry = rowMov[playerMove.move];
            char dirx = columnMov[playerMove.move];

            masterEntrySync(sync);

            // región crítica de escritura
            validMove = processMove(state, playerMove.playerIndex, dirx, diry);
            
            masterExitSync(sync);

            // TODO acá mismo agregar lo de levantar isBlocked si el jugador que jugó está bloqueado

            moveProcessedPostSync(sync, playerMove.playerIndex);

            // TOOO imprimir solo is hubo cambios ?
        }



        if(time(NULL) - lastValidMoveTime > params.timeout)
        {
            state->isGameOver = true;
        }

        if(validMove)
        {
            lastValidMoveTime = time(NULL);
        }
        

    }


    // TODO post/signal a view para el último print


    // TODO wait para no dejar zombies 
    // TODO guardar e imprimir valores de retorno de jugadores y view


    // libero pipes
    freePipes(pipesfd, state->numPlayers);

    freeGameState(state);
    freeGameSync(sync);
    
    closeSHM(state, sizeof(GameState) + params.width * params.height * sizeof(int));
    closeSHM(sync, sizeof(GameSync));
    unlinkSHM(GAME_STATE_SHM);
    unlinkSHM(GAME_SYNC_SHM);

    

    return 0;
}


// TODO debug
//solo de testeo
void printParams(MasterParameters params) {
    printf("width=%d\theight=%d\tdelay=%d\ttimeout=%d\tseed=%ld\tview=%s\tnumPlayers=%d\n", params.width, params.height, params.delay, params.timeout, params.seed, params.view, params.numPlayers);
    for(int i=0; i<params.numPlayers; i++){
        printf("player %d 's name=%s\n", i+1, params.players[i]);
    }
    
}

