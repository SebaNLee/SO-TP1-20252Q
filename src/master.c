
#include "master.h"
#include "structs.h"






int main(int argc, char const *argv[]) {

    const char *GAME_STATE_SHM = "/game_state";
    const char *GAME_SYNC_SHM = "/game_sync";

    // parseo de parámetros
    MasterParameters params = setParams(argc, ( char * const *) argv);

    // TODO debug
    printParams(params);

    size_t boardSize = params.width * params.height * sizeof(int);
    size_t stateSize = sizeof(GameState) + boardSize;
    size_t syncSize = sizeof(GameSync);

    GameState * state = (GameState*) createSHM(GAME_STATE_SHM, stateSize);
    GameSync * sync = (GameSync *) createSHM(GAME_SYNC_SHM, syncSize);

    initGameState(state, params);
    initGameSync(sync);
    
    // incializo pipes para cada jugador
    int pipesfd[state->numPlayers][2];
    initPipes(pipesfd, state->numPlayers);

    // la idea es que quiero reemplazar los fd de STDOUT y STDIN de los players por lso fd del pide del master 
    // fork por cada jugador
    // dup2 para reemplazar STDOUT de cada jugador

    // inicializo jugadores y vista
    initPlayers(params, state, pipesfd);
    initView(params);



    // ETC


    while(!state->isGameOver)
    {
        // TODO select() para esperar lecturas de pipes
        // TODO semáforos
        // TODO chequeo de movimiento válido
        // TODO chequeo de tiempo (si levanto isGameOver o no)
        // TODO delay



        // TODO debug
        
        unsigned char move;
        for (int i = 0; i < state->numPlayers; i++) {

            // hardcodeo, levanto semáforos
            sem_post(&sync->send_move[i]);

            // creo que por acá iría el select()

            int n =read(pipesfd[i][PIPE_READ_END], &move, sizeof(move));
            if(n > 0)
                printf("Jugador %d hizo jugada: %d\n", i, move);

            // por ahora funciona para el jugador 0, pero el pipe está andando
            // creo que falla por lo de select()

            // hardcodeo, view imprime una vez
            // TODO creo que no anda por lo de MUTEX
            sem_post(&sync->view_reading_pending);

        }
        //



    }


    // ETC




    // libero pipes
    freePipes(pipesfd, state->numPlayers);

    freeSemaphores(sync);
    // freeGameState(state); // TODO



    // TODO borrar debug
    printf("OK!\n");

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

void initGameState(GameState * state, MasterParameters params)
{


    // Llenar información del gameState
    
    state->width = params.width;
    state->height = params.height;
    state->numPlayers = params.numPlayers;
    state->isGameOver = false;

    memset(state->players, 0, sizeof(state->players));
    memset(state->board, 0, params.width * params.height * sizeof(int));



    printf("Se creó la memoria compartida!\n");
    printf("Creando tablero...\n");

    // Generación de tablero
    srand(time(NULL));

    for (int i = 0; i < state->width * state->height; i++) {
        state->board[i] = (rand() % 9) + 1; 
    }

    printf("Tablero creado!\n");

    for (int i=0; i < state->height; i++) {
        for (int j=0; j < state->width; j++) {
            printf("%d ", state->board[i*state->width+j]);
        }
        printf("\n");
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
    if (sem_init(&sync->mutex_readers, 1, 1) == ERROR) {
         perror("sem_init   mutex_readers\n"); 
        exit(1); 
    } 
    if (sem_init(&sync->mutex_writer, 1, 1) == ERROR) {
         perror("sem_init mutex_writer\n");
         exit(1); 
        }
    if (sem_init(&sync->mutex_counter, 1, 1) == ERROR) {
         perror("sem_init mutex_counter\n"); 
         exit(1);
         }

    // Contador de lectores
    sync->numReaders = 0;

    // Turnos de jugadores
    for (int i = 0; i < 9; i++) {
        if (sem_init(&sync->send_move[i], 1, 0) == ERROR) {
            perror("sem_init send_move[i]\n");
            exit(1);
        }
    }

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
    

int freeSemaphores(GameSync * sync) {
   if( sem_destroy(&sync->view_reading_pending)==ERROR){
    perror("sem_destroy view_reading_pending\n");
    return ERROR;
   }
    if( sem_destroy(&sync->view_writing_done)==ERROR){
    perror("sem_destroy view_writing_done\n");
    return ERROR;
   }
    if( sem_destroy(&sync->mutex_readers)==ERROR){
    perror("sem_destroy mutex_readers\n");
    return ERROR;
   }
    if( sem_destroy(&sync->mutex_writer)==ERROR){
    perror("sem_destroy mutex_writer\n");
    return ERROR;
   }
   if( sem_destroy(&sync->mutex_counter)==ERROR){
    perror("sem_destroy mutex_counter\n");
    return ERROR;
   }
   for (int i = 0; i < 9; i++) {
        if (sem_destroy(&sync->send_move[i]) == ERROR) {
            perror("sem_destroy send_move[i]\n");
            return ERROR;
        }
    }

    return SUCCESS;
}



// TODO debug
//solo de testeo
void printParams(MasterParameters params) {
    printf("width=%d\theight=%d\tdelay=%d\ttimeout=%d\tseed=%ld\tview=%s\tnumPlayers=%d\n", params.width, params.height, params.delay, params.timeout, params.seed, params.view, params.numPlayers);
    for(int i=0; i<params.numPlayers; i++){
        printf("player %d 's name=%s\n", i+1, params.players[i]);
    }
    
}

