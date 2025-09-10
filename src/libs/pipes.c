#include "pipes.h"




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