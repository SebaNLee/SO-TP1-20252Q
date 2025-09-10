#include "init.h"
#include "structs.h"
#include "parameters.h"



void initGameState(GameState * state, MasterParameters params)
{
    state->width = params.width;
    state->height = params.height;
    state->numPlayers = params.numPlayers;
    state->isGameOver = false;

    memset(state->players, 0, sizeof(state->players));
    memset(state->board, 0, params.width * params.height * sizeof(int));

    // Generación de tablero
    srand(time(NULL));

    for (int i = 0; i < state->width * state->height; i++) {
        state->board[i] = (rand() % 9) + 1; 
    }
}

void initGameSync(GameSync * sync)
{
    // Master <-> Vista
    if (sem_init(&sync->view_reading_pending, 1, 0) == ERROR) {
         perror("sem_init view_reading_pending\n");
         exit(1); 
        }
    if (sem_init(&sync->view_writing_done, 1, 0) == ERROR) {
         perror("sem_init view_writing_done\n");
         exit(1); 
        }

    // Lectores <-> Escritor
    if (sem_init(&sync->mutex_master_access, 1, 1) == ERROR) {
         perror("sem_init   mutex_master_access\n"); 
        exit(1); 
    } 
    if (sem_init(&sync->mutex_game_state_access, 1, 1) == ERROR) {
         perror("sem_init mutex_game_state_access\n");
         exit(1); 
        }
    if (sem_init(&sync->mutex_readers_counter, 1, 1) == ERROR) {
         perror("sem_init mutex_readers_counter\n"); 
         exit(1);
         }

    // Contador de lectores
    sync->readers_counter = 0;

    // Turnos de jugadores
    for (int i = 0; i < 9; i++) {
        if (sem_init(&sync->move_processed[i], 1, 0) == ERROR) {
            perror("sem_init send_move[i]\n");
            exit(1);
        }
    }
}


int freeGameSync(GameSync * sync) {
   if( sem_destroy(&sync->view_reading_pending)==ERROR){
    perror("sem_destroy view_reading_pending\n");
    return ERROR;
   }
    if( sem_destroy(&sync->view_writing_done)==ERROR){
    perror("sem_destroy view_writing_done\n");
    return ERROR;
   }
    if( sem_destroy(&sync->mutex_master_access)==ERROR){
    perror("sem_destroy mutex_master_access\n");
    return ERROR;
   }
    if( sem_destroy(&sync->mutex_game_state_access)==ERROR){
    perror("sem_destroy mutex_game_state_access\n");
    return ERROR;
   }
   if( sem_destroy(&sync->mutex_readers_counter)==ERROR){
    perror("sem_destroy mutex_readers_counter\n");
    return ERROR;
   }
   for (int i = 0; i < 9; i++) {
        if (sem_destroy(&sync->move_processed[i]) == ERROR) {
            perror("sem_destroy send_move[i]\n");
            return ERROR;
        }
    }

    return SUCCESS;
}


void initPlayers(MasterParameters params, GameState * state, int pipesfd[][2])
{
    for(int i = 0; i < state->numPlayers; i++)
    {
        int pid = fork();

        if(pid < 0)
        {
            perror("fork() error\n");
            exit(1);
        }
        else if(pid == 0) // hijo
        {
            // consigna: "El máster garantiza que el extremo de escritura de este pipe esté asociado al descriptor de archivo 1"

            // cierro read end del pide del player
            close(pipesfd[i][PIPE_READ_END]);

            // copia write end del pipe al STDOUT del player 
            if(dup2(pipesfd[i][PIPE_WRITE_END], STDOUT_FILENO) == -1)
            {
                perror("dup2() error\n");
                exit(1);
            }

            // cierro el write end del pipe original (ya está en STDOUT)
            close(pipesfd[i][PIPE_WRITE_END]);

            // preparo argumentos para player
            char widthStr[16];
            char heightStr[16];
            snprintf(widthStr, sizeof(widthStr), "%d", params.width);
            snprintf(heightStr, sizeof(heightStr), "%d", params.height);
            char * playerArgv[] = {params.players[i], widthStr, heightStr, NULL};

            // ejecuto proceso player
            execv(params.players[i], playerArgv); // TODO tal vez esto es execve? no entiendo la parte de envp ??
        }
        else // padre
        {
            // me guardo el PID del hijo (jugador i-ésimo)
            state->players[i].pid = pid;
        }
    }

    return;
}


void initView(MasterParameters params)
{
    // análogo a initPlayers, sin lo de pipes

    int pid = fork();

    if(pid < 0)
    {
        perror("fork() error\n");
        exit(1);
    }
    else if(pid == 0) // hijo
    {
        char widthStr[16];
        char heightStr[16];
        snprintf(widthStr, sizeof(widthStr), "%d", params.width);
        snprintf(heightStr, sizeof(heightStr), "%d", params.height);
        char * viewArgv[] = {params.view, widthStr, heightStr, NULL};

        execv(params.view, viewArgv); // TODO tal vez esto es execve? no entiendo la parte de envp ??
    }
    
    return;
}


void initPipes(int pipesfd[][2], int numPlayers)
{
    for(int i = 0; i < numPlayers; i++)
    {
        // creo pipes y obtengo fds
        if(pipe(pipesfd[i]) == -1)
        {
            perror("Error inicializando pipes\n");
            exit(1);
        }
    }

    return;
}


void freePipes(int pipesfd[][2], int numPlayers)
{
    for(int i = 0; i < numPlayers; i++)
    {
        close(pipesfd[i][PIPE_READ_END]);
        close(pipesfd[i][PIPE_WRITE_END]);
    }

    return;
}