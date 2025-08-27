
#include "master.h"
#include "structs.h"






int main(int argc, char const *argv[]) {

    // parseo de parámetros
    MasterParameters params = setParams(argc, ( char * const *) argv);

    // TODO debug
    printParams(params);

    GameState * state = initGameState(params);

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

   initSemaphores(sync);

    
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

    freeSemaphores(sync);
}

GameState * initGameState(MasterParameters parameters)
{
    // TODO aca iniciar y devolver shmem
    const char * memory = "shm";
    const size_t boardSize = parameters.width * parameters.height * sizeof(int);
    const size_t size = sizeof(GameState) + boardSize;

    // Creación de memoria
    int fd = shm_open(memory, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Failed to create shared memory");
        exit(1);
    }
    
    // Ajuste de tamaño
    ftruncate(fd, size);

    // Mapeo
    void * ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Failed to map memory");
        exit(1);
    }

    // Ya no lo necesitamos
    close(fd);


    // Llenar información del gameState
    GameState * state = (GameState*) ptr;
    state->width = parameters.width;
    state->height = parameters.height;
    state->numPlayers = parameters.numPlayers;
    state->isGameOver = false;

    memset(state->players, 0, sizeof(state->players));
    memset(state->board, 0, boardSize);



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

    // TODO debug
    return state;
}

GameSync * initGameSync()
{
    // TODO aca iniciar y devolver shmem


    // TODO debug
    return NULL;
}

int initSemaphores(GameSync * sync){

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
void printParams(MasterParameters params){
    printf("width=%d\theight=%d\tdelay=%d\ttimeout=%d\tseed=%ld\tview=%s\tnumPlayers=%d\n", params.width, params.height, params.delay, params.timeout, params.seed, params.view, params.numPlayers);
    for(int i=0; i<params.numPlayers; i++){
        printf("player %d 's name=%s\n", i+1, params.players[i]);
    }
    
}

