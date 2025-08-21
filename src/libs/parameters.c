#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "parameters.h"


#define ERROR -1
#define SUCCESS 1
#define MINPLAYERS 1
#define MAXPLAYERS 9
//hay que ver de hacer capaz una struct con parametros default para que los usuarios puedan correrlo sin los argumentos opcionales y no se rompan las cosas
//por ahora lo deje en variables y no un struct

int width = 10;
int height = 10;
int delay =200;
int timeout = 10;
long seed;
char* view; //que tipo de dato es vista?
int numPlayers=0;
char *players[MAXPLAYERS];

extern char *optarg;
extern int optind, opterr, optopt;



//solo de testeo
void printParams(){
    printf("width=%d\theight=%d\tdelay=%d\ttimeout=%d\tseed=%ld\tview=%s\tnumPlayers=%d\n", width, height, delay, timeout, seed, view, numPlayers);
    for(int i=0; i<numPlayers; i++){
        printf("player %d 's name=%s\n", i+1, players[i]);
    }
    
}

int setParams(int argc, char* const  argv[]){
    seed= time(NULL);
     int op;
    if (argc < 2) {
        fprintf(stderr, "Error: At least one player must be specified using -p.\n");
        return ERROR;
    }
    while ((op = getopt(argc, argv, "p:w:h:d:t:s:v:")) != ERROR) {
        switch (op) {
            case 'w':
                if (atoi(optarg) >= 10) {
                    width = atoi(optarg);
                }
                break;
            case 'h':
                if (atoi(optarg) >= 10) {
                    height= atoi(optarg);
                }
                break;
            case 'd':
                if (atoi(optarg) > 0) {
                    delay = atoi(optarg);
                }
                break;
            case 's':
                if (atoi(optarg) > 0) {
                    seed = atoi(optarg);
                }
                break;
            case 'v':
                view = optarg;
                break;
            case 't':
                if (atoi(optarg) > 0) {
                    timeout = atoi(optarg);
                }
                break;
            case 'p': {
                int idx = optind - 1;
                while (argv[idx] != NULL && argv[idx][0] != '-') {
                    if (numPlayers >= MAXPLAYERS) {
                        fprintf(stderr, "Error: At most 9 players can be specified using -p");
                        return ERROR;
                    }
                    players[numPlayers]= argv[idx];
                    numPlayers++;
                    idx++;
                   
                }
                break;
            }
            default:
                return ERROR;
        }
    }
    if ( numPlayers<=0) {
        fprintf(stderr, "Error: At least one player must be specified using -p.\n");
        return ERROR;
    }
    return SUCCESS;
}