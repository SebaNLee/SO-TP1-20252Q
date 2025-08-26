
#include "master.h"
#include "structs.h"






int main(int argc, char const *argv[]) {


    // parseo de parámetros
    MasterParameters params = setParams(argc, ( char * const *) argv);

    // TODO debug
    printParams(params);

    GameState * state = initGameState();

    GameSync * sync = initGameSync();

    // TODO dummy para warning
    (void) sync; // borrar
    (void) state; // borrar


    // TODO tal vez sea mejor init shmem primero, para después parsear params directamente en state y sync

    // paso parámetros a shmem
    state->width = params.width;
    state->height = params.height;
    state->numPlayers = params.numPlayers;
    state->width = params.width;

    initSemaphones(sync);

    
    // incializo pipes para cada jugador
    int pipesfd[state->numPlayers][2];

    // la idea es que quiero reemplazar los fd de STDOUT y STDIN de los players por lso fd del pide del master 
    for(int i = 0; i < state->numPlayers; i++)
    {
        // creo pipes y obtengo fds
        if(pipe(pipesfd[i]) == -1)
        {
            perror("Error inicializando pipes\n");
            exit(1);
        }


        // fork por cada jugador
        // dup2 para reemplazar STDOUT de cada jugador
    }


    // ETC


    // cierro pipes
    for(int i = 0; i < state->numPlayers; i++)
    {
        close(pipesfd[i][PIPE_READ_END]);
        close(pipesfd[i][PIPE_WRITE_END]);
    }
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

int initSemaphones(GameSync * sync){

    // Master <-> Vista
    if (sem_init(&sync->view_reading_pending, 1, 0) == ERROR) {
         perror("sem_init view_reading_pending\n");
         return ERROR; 
        }
    if (sem_init(&sync->view_writing_done, 1, 0) == ERROR) {
         perror("sem_init view_writing_done\n");
         return ERROR; 
        }

    // Lectores <-> Escritor
    if (sem_init(&sync->mutex_readers, 1, 1) == ERROR) {
         perror("sem_init   mutex_readers\n"); 
        return ERROR; 
    } 
    if (sem_init(&sync->mutex_writer, 1, 1) == ERROR) {
         perror("sem_init mutex_writer\n");
         return ERROR; 
        }
    if (sem_init(&sync->mutex_counter, 1, 1) == ERROR) {
         perror("sem_init mutex_counter\n"); 
         return ERROR;
         }

    // Contador de lectores
    sync->numReaders = 0;

    // Turnos de jugadores
    for (int i = 0; i < 9; i++) {
        if (sem_init(&sync->send_move[i], 1, 0) == ERROR) {
            perror("sem_init send_move[i]\n");
            return ERROR;
        }
    }

    return SUCCESS;

}



// TODO debug
//solo de testeo
void printParams(MasterParameters params){
    printf("width=%d\theight=%d\tdelay=%d\ttimeout=%d\tseed=%ld\tview=%s\tnumPlayers=%d\n", params.width, params.height, params.delay, params.timeout, params.seed, params.view, params.numPlayers);
    for(int i=0; i<params.numPlayers; i++){
        printf("player %d 's name=%s\n", i+1, params.players[i]);
    }
    
}

