#include "init.h"
#include "structs.h"
#include "parameters.h"

void initGameState(GameState *state, MasterParameters params)
{
    state->width = params.width;
    state->height = params.height;
    state->numPlayers = params.numPlayers;
    state->isGameOver = false;

    memset(state->players, 0, sizeof(state->players));
    memset(state->board, 0, params.width * params.height * sizeof(int));

    // Generación de tablero
    srand(params.seed);

    for (int i = 0; i < state->width * state->height; i++)
    {
        state->board[i] = (rand() % 9) + 1;
    }
}

void initGameSync(GameSync *sync)
{
    // Master <-> Vista
    if (sem_init(&sync->view_reading_pending, 1, 0) == ERROR)
    {
        perror("sem_init view_reading_pending\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&sync->view_writing_done, 1, 0) == ERROR)
    {
        perror("sem_init view_writing_done\n");
        exit(EXIT_FAILURE);
    }

    // Lectores <-> Escritor
    if (sem_init(&sync->mutex_master_access, 1, 1) == ERROR)
    {
        perror("sem_init   mutex_master_access\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&sync->mutex_game_state_access, 1, 1) == ERROR)
    {
        perror("sem_init mutex_game_state_access\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&sync->mutex_readers_counter, 1, 1) == ERROR)
    {
        perror("sem_init mutex_readers_counter\n");
        exit(EXIT_FAILURE);
    }

    // Contador de lectores
    sync->readers_counter = 0;

    // Turnos de jugadores
    for (int i = 0; i < 9; i++)
    {
        if (sem_init(&sync->move_processed[i], 1, 0) == ERROR)
        {
            perror("sem_init send_move[i]\n");
            exit(EXIT_FAILURE);
        }
    }
}

void freeGameSync(GameSync *sync)
{
    if (sem_destroy(&sync->view_reading_pending) == ERROR)
    {
        perror("sem_destroy view_reading_pending\n");
        exit(EXIT_FAILURE);
    }
    if (sem_destroy(&sync->view_writing_done) == ERROR)
    {
        perror("sem_destroy view_writing_done\n");
        exit(EXIT_FAILURE);
    }
    if (sem_destroy(&sync->mutex_master_access) == ERROR)
    {
        perror("sem_destroy mutex_master_access\n");
        exit(EXIT_FAILURE);
    }
    if (sem_destroy(&sync->mutex_game_state_access) == ERROR)
    {
        perror("sem_destroy mutex_game_state_access\n");
        exit(EXIT_FAILURE);
    }
    if (sem_destroy(&sync->mutex_readers_counter) == ERROR)
    {
        perror("sem_destroy mutex_readers_counter\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 9; i++)
    {
        if (sem_destroy(&sync->move_processed[i]) == ERROR)
        {
            perror("sem_destroy send_move[i]\n");
            exit(EXIT_FAILURE);
        }
    }

    return;
}

void initPlayers(MasterParameters params, GameState *state, int pipesfd[][2])
{
    for (int i = 0; i < state->numPlayers; i++)
    {
        int pid = fork();

        if (pid < 0)
        {
            perror("fork() error\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) // hijo
        {
            // consigna: "El máster garantiza que el extremo de escritura de este pipe esté asociado al descriptor de archivo 1"

            // cierro read end del pide del player
            if (close(pipesfd[i][PIPE_READ_END]) == ERROR)
            {
                perror("Error in close");
                exit(EXIT_FAILURE);
            }

            // copia write end del pipe al STDOUT del player
            if (dup2(pipesfd[i][PIPE_WRITE_END], STDOUT_FILENO) == ERROR)
            {
                perror("dup2() error\n");
                exit(EXIT_FAILURE);
            }

            // cierro el write end del pipe original (ya está en STDOUT)
            if (close(pipesfd[i][PIPE_WRITE_END]) == ERROR)
            {
                perror("Error in close");
                exit(EXIT_FAILURE);
            }

            // preparo argumentos para player
            char widthStr[16];
            char heightStr[16];
            snprintf(widthStr, sizeof(widthStr), "%d", params.width);
            snprintf(heightStr, sizeof(heightStr), "%d", params.height);
            char *playerArgv[] = {params.players[i], widthStr, heightStr, NULL};

            // ejecuto proceso player
            if (execv(params.players[i], playerArgv) == ERROR)
            { // TODO tal vez esto es execve? no entiendo la parte de envp ??
                perror("Error in execv");
                exit(EXIT_FAILURE);
            }
        }
        else // padre
        {
            // me guardo el PID del hijo (jugador i-ésimo)
            state->players[i].pid = pid;
            char *slash = strrchr(params.players[i], '/');
            const char *basename = slash ? slash + 1 : params.players[i];
            strncpy(state->players[i].name, basename, sizeof(state->players[i].name) - 1);
            state->players[i].name[sizeof(state->players[i].name) - 1] = '\0';
        }
    }

    return;
}

int initView(MasterParameters params)
{
    // análogo a initPlayers, sin lo de pipes

    int pid = fork();

    if (pid < 0)
    {
        perror("fork() error\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // hijo
    {
        char widthStr[16];
        char heightStr[16];
        snprintf(widthStr, sizeof(widthStr), "%d", params.width);
        snprintf(heightStr, sizeof(heightStr), "%d", params.height);
        char *viewArgv[] = {params.view, widthStr, heightStr, NULL};

        if (execv(params.view, viewArgv) == ERROR)
        { // TODO tal vez esto es execve? no entiendo la parte de envp ??
            perror("Error in execv");
            exit(EXIT_FAILURE);
        }
    }

    return pid;
}

void initPipes(int pipesfd[][2], int numPlayers)
{
    for (int i = 0; i < numPlayers; i++)
    {
        // creo pipes y obtengo fds
        if (pipe(pipesfd[i]) == ERROR)
        {
            perror("Error inicializando pipes\n");
            exit(EXIT_FAILURE);
        }
    }

    return;
}

void freePipes(int pipesfd[][2], int numPlayers)
{
    for (int i = 0; i < numPlayers; i++)
    {
        if ((close(pipesfd[i][PIPE_READ_END]) == ERROR) || (close(pipesfd[i][PIPE_WRITE_END]) == ERROR))
        {
            perror("Error in close");
            exit(EXIT_FAILURE);
        }
    }

    return;
}

void waitViewAndPlayers(GameState *state, int viewPID)
{
    int wstatus;

    // view
    waitpid(viewPID, &wstatus, 0);
    if (WIFEXITED(wstatus))
    {
        printf("View exited (%d)\n", WEXITSTATUS(wstatus));
    }

    // players
    for (size_t i = 0; i < state->numPlayers; i++)
    {
        waitpid(state->players[i].pid, &wstatus, 0);
        if (WIFEXITED(wstatus))
        {
            printf("Player %s (%ld) exited (%d) with a score of %d / %d / %d\n", state->players[i].name, i, WEXITSTATUS(wstatus), state->players[i].score, state->players[i].validMoves, state->players[i].invalidMoves);
        }
    }
}