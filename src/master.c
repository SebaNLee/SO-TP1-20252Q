
#include "master.h"
#include <sys/select.h>   
#include "structs.h"
#include <math.h>

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

    size_t boardSize = params.width * params.height * sizeof(int);
    size_t stateSize = sizeof(GameState) + boardSize;
    size_t syncSize = sizeof(GameSync);

    GameState * state = (GameState*) createSHM(GAME_STATE_SHM, stateSize, true, true);
    GameSync * sync = (GameSync *) createSHM(GAME_SYNC_SHM, syncSize, true, true);

    initGameState(state, params);
    initGameSync(sync);
    
    // incializo pipes para cada jugador
    int pipesfd[state->numPlayers][2];
    initPipes(pipesfd, state->numPlayers);

    // inicializo jugadores y vista
    initPlayers(params, state, pipesfd);
    initView(params);
    setPlayerPosition(state, state->width, state->height, state->numPlayers);
    
    
    int lastProcessedPlayer = 0;

    // ETC




    

    while(!state->isGameOver)
    {

        // TODO delay

        sem_post(&sync->view_reading_pending);
        sem_wait(&sync->view_writing_done);

        
        // debug
        for (int i = 0 ; i < state->numPlayers; i++) { 
            sem_post(&sync->send_move[i]); 
        }
        

        // Crear el set de pipes que se leen en select
        fd_set fds;
        FD_ZERO(&fds);

        // Cálculo de maxfd
        int maxfd = 0;
        for (int i = 0; i < state->numPlayers; i++) {
            if (pipesfd[i][PIPE_READ_END] > maxfd)
                maxfd = pipesfd[i][PIPE_READ_END];
        }

        for (int i = 0; i < state->numPlayers; i++) {
            if (!state->players[i].isBlocked)
                FD_SET(pipesfd[i][PIPE_READ_END], &fds);
        }

        // Seteo de timeout base
        struct timeval timeInterval = {.tv_sec = params.timeout, .tv_usec = 0}; 
        
        // Esperar movimiento de algún jugador
        int activity = select(maxfd + 1, &fds, NULL, NULL, &timeInterval);
        /*
           Qué pasa con select? 
           Supongamos que tenemos 3 jugadores con fd [4, 5, 6]. Si solo el fd 5 tiene datos, despues del select el fds queda únicamente con el 5. Los otros desaparecen.
            Con el intervalo de tiempo pasa lo mismo, lo modifica. 
        */
        if (activity < 0) {
            perror("Failed in function select");
            break;
        }

        int start = (lastProcessedPlayer++) % state->numPlayers;

        printf("Llegamos hasta aca!\n");

        // TODO mutex y escribo
        // TODO check: inanición, timeout, isBlocked, mutex, G[]
        for (int offset = 0; offset < state->numPlayers; offset++) {
            int i = (start + offset) % state->numPlayers;

            if (!state->players[i].isBlocked && FD_ISSET(pipesfd[i][PIPE_READ_END], &fds)) {
                unsigned char move;
                int n = read(pipesfd[i][PIPE_READ_END], &move, sizeof(move));
                printf("Procesando movimientos...\n");
                // REVISAR USO
                if (n == 0) {
                    state->players[i].isBlocked = 1;
                } else if (n < 0) {
                    perror("Failed to read");
                } else {
                    processPlayerMove(state, sync, i, move);

                    sem_post(&sync->send_move[i]);
                    
                }
            }

        }

        // TODO libero mutex


    }


    // TODO post/signal a view para el último print


    // TODO wait para no dejar zombies 



    // libero pipes
    freePipes(pipesfd, state->numPlayers);

    freeSemaphores(sync);
    
    closeSHM(state, stateSize);
    closeSHM(sync, syncSize);
    unlinkSHM(GAME_STATE_SHM);
    unlinkSHM(GAME_SYNC_SHM);


    // TODO borrar debug
    printf("OK!\n");

}


void processPlayerMove( GameState * state, GameSync * sync, int i, unsigned char move ) {
    char diry = rowMov[move];
    char dirx = columnMov[move];

    sem_wait(&sync->mutex_writer);

    validateMove(state, i, dirx, diry);
    
    sem_post(&sync->mutex_writer);

}

void validateMove(GameState * state, int i, char dirx, char diry) {

    int finalXpos = state->players[i].x + dirx;
    int finalYpos = state->players[i].y + diry;
    if ( finalXpos < 0 || finalXpos >= state->width || finalYpos < 0 || finalYpos >= state->height || state->board[finalXpos + state->width * finalYpos] <= 0) {
        state->players[i].invalidMoves++;
    } else {
        state->players[i].score += state->board[finalXpos + state->width * finalYpos];
        state->players[i].validMoves++;
        state->board[finalXpos + state->width * finalYpos] = -i;
        state->players[i].x = finalXpos;
        state->players[i].y = finalYpos;
    }

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

//separa el tablero por regiones y le asigna el centro de cada region a un jugador
void setPlayerPosition(GameState * state, int width, int height, int numPlayers){
  
    int rows, cols;


    rows = cols = (int)ceil(sqrt(numPlayers));

    int idx = 0;
    for (int r = 0; r < rows && idx < numPlayers; r++) {
        for (int c = 0; c < cols && idx < numPlayers; c++) {
            
            int x = (c * width / cols) + (width / (2 * cols));
            int y = (r * height / rows) + (height / (2 * rows));

            state->players[idx].x = x;
            state->players[idx].y = y;

            // marcar celda como tomada por este jugador
            state->board[y * width + x] = -(idx+1); 

            idx++;
        }
    }

}



// TODO debug
//solo de testeo
void printParams(MasterParameters params) {
    printf("width=%d\theight=%d\tdelay=%d\ttimeout=%d\tseed=%ld\tview=%s\tnumPlayers=%d\n", params.width, params.height, params.delay, params.timeout, params.seed, params.view, params.numPlayers);
    for(int i=0; i<params.numPlayers; i++){
        printf("player %d 's name=%s\n", i+1, params.players[i]);
    }
    
}

