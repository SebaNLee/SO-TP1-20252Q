#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "parameters.h"



extern char *optarg;
extern int optind, opterr, optopt;


//hay que ver de hacer capaz una struct con parametros default para que los usuarios puedan correrlo sin los argumentos opcionales y no se rompan las cosas
MasterParameters setParams(int argc, char* const  argv[]){

    // default values (se pisan luego si hubo ingreso)
    MasterParameters params = { .width = 10,
                                .height = 10,
                                .delay =200,
                                .timeout = 10,
                                .numPlayers = 0,
                                .seed = time(NULL) 
                            };

    int op;

    if (argc < 2) {
        fprintf(stderr, "Error: At least one player must be specified using -p.\n");
        exit(1);
    }

    while ((op = getopt(argc, argv, "p:w:h:d:t:s:v:")) != -1) {
        switch (op) {
            case 'w':
                   if (atoi(optarg) < 10) { 
                    fprintf(stderr,"Error: Width selected must be of at least 10\n");
                    exit(1);
                }
                params.width = atoi(optarg);
                break;
            case 'h':
                   if (atoi(optarg) < 10) { 
                    fprintf(stderr,"Error: Height selected must be of at least 10\n");
                    exit(1);
                }
                params.height = atoi(optarg);
                break;
            case 'd':
                if (atoi(optarg) <= 0) {
                  fprintf(stderr,"Error: Delay selected must be a positive number\n");
                  exit(1);
                }
                params.delay = atoi(optarg);
                break;
            case 's':
                if (atoi(optarg) <= 0) {
                     fprintf(stderr,"Error: Seed selected must be a positive number\n");
                  exit(1);
                }
                params.seed = atoi(optarg);
                break;
            case 'v':
                params.view = optarg;
                break;
            case 't':
                if (atoi(optarg) <= 0) {
                    fprintf(stderr,"Error: Timeout selected must be a positive number\n");
                  exit(1);
                }
                params.timeout = atoi(optarg);
                break;
            case 'p': {
                int idx = optind - 1;
                while (argv[idx] != NULL && argv[idx][0] != '-') {
                    if (params.numPlayers >= MAXPLAYERS) {
                        fprintf(stderr, "Error: At most 9 players can be specified using -p\n");
                        exit(1);
                    }
                    params.players[params.numPlayers]= argv[idx];
                    params.numPlayers++;
                    idx++;
                   
                }
                break;
            }
            default:
                exit(1);
        }
    }

    if (params.numPlayers<=0) {
        fprintf(stderr, "Error: At least one player must be specified using -p.\n");
        exit(1);
    }

    return params;
}