
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


    // inicializaciones para el ciclo principal
    int lastProcessedPlayer = 0;

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


        // Crear el set de pipes que se leen en select
        fd_set fds;
        FD_ZERO(&fds);

        // Cálculo de maxfd
        int maxfd = 0;
        for (int i = 0; i < state->numPlayers; i++) {
            if (pipesfd[i][PIPE_READ_END] > maxfd)
                maxfd = pipesfd[i][PIPE_READ_END];
        }

        // meto al set los pipes de los jugadores activos
        for (int i = 0; i < state->numPlayers; i++) {
            if (!state->players[i].isBlocked)
                FD_SET(pipesfd[i][PIPE_READ_END], &fds);
        }

        // Seteo de timeout base
        struct timeval timeInterval = {.tv_sec = abs(params.timeout - (time(NULL) - startTime)), .tv_usec = 0}; // TODO cálculo de milisegundos?

        // chequear si algún jugador mandó movimiento
        int activity = select(maxfd + 1, &fds, NULL, NULL, &timeInterval);
        if (activity < 0)
        {
            perror("Failed in function select");
            exit(1);
        }
        // no hubo writes de jugadores en tiempo timeout (entonces salgo)
        else if(activity == 0)
        {
            state->isGameOver = true;
        }
        // si hubo devoluciones, agarro con round robin al primer fd con datos
        else if(activity > 0)
        {
            int start = (lastProcessedPlayer++) % state->numPlayers;

            for (int offset = 0; offset < state->numPlayers; offset++) {
                
                int i = (start + offset) % state->numPlayers;

                if (!state->players[i].isBlocked && FD_ISSET(pipesfd[i][PIPE_READ_END], &fds)) {
                    
                    unsigned char move;
                    int n = read(pipesfd[i][PIPE_READ_END], &move, sizeof(move));

                    // REVISAR USO
                    if (n < 0)
                    {
                        perror("Failed to read");
                        exit(1);
                    }
                    else
                    {
                        // TODO chequear EOF en consigna
                        if(move == EOF)
                        {
                            state->players[i].isBlocked = 1;
                        }
                        else
                        {
                            char diry = rowMov[move];
                            char dirx = columnMov[move];

                            masterEntrySync(sync);

                            // región crítica de escritura
                            validMove = processMove(state, i, dirx, diry);
                            
                            masterExitSync(sync);

                            // TODO acá mismo agregar lo de levantar isBlocked si el jugador que jugó está bloqueado

                            moveProcessedPostSync(sync, i);

                            break;

                            // TOOO imprimir solo is hubo cambios ?
                        }
                        
                    }
                }

            }

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

