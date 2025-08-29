#include "view.h"

#define RESET   "\033[0m"
#define RED     "\033[41m"
#define GREEN   "\033[42m"
#define YELLOW  "\033[43m"
#define BLUE    "\033[44m"
#define MAGENTA "\033[45m"
#define CYAN    "\033[46m"
#define WHITE   "\033[47m"
#define BRIGHT_BLUE     "\033[104m"
#define BRIGHT_MAGENTA   "\033[105m"
#define GOLD_BG   "\033[48;2;212;175;55m"   
#define SILVER_BG "\033[48;2;192;192;192m"  
#define BRONZE_BG "\033[48;2;205;127;50m"   


const char* colors[] = {RED, BLUE, GREEN, YELLOW, MAGENTA, CYAN, WHITE, BRIGHT_BLUE, BRIGHT_MAGENTA, GOLD_BG, SILVER_BG, BRONZE_BG};

void printHeader(int columns) {
        int counter = 1;
        printf("╔");
        while (counter++ < columns) {
            printf("═══╦");
        }
        printf("═══╗\n");
}

void printTableContent(GameState * state, int width, int height) {

    

    int numPlayers = state->numPlayers;
    int * board = state->board;
    Player * players = state->players;
    
    
    printHeader(width);
    

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            
            int val = board[i * width + j];
            char isHead = 0;
            for (int p = 0; p < numPlayers; p++) {
                if (players[p].x == j && players[p].y == i)
                    isHead = 1;
            }

            if(val <= 0) {
                if (isHead) {
                    printf("║%s🦆 %s", colors[abs(val)], RESET);
                } else {
                    printf("║%s   %s", colors[abs(val)], RESET); 
                }
            } else {
                printf("║ %d ", val);
            }

            
        }
        printf("║\n");
        if (i+1 < state->height) {
            printf("╠═══");
            for (int j = 1; j < width; j++) {
                printf("╬═══");
            }
            printf("╣\n");
        } 
    }

    printBase(width);
}

void printBase(int columns) {
    int counter = 1;
        printf("╚");
        while (counter++ < columns) {
            printf("═══╩");
        }
        printf("═══╝\n");
}

int comparePlayersPositions(const void *a, const void *b) {

    const Player *p1 = *(const Player**)a;
    const Player *p2 = *(const Player**)b;

    if (p1->score != p2->score)
        return p2->score - p1->score;
    if (p1->validMoves != p2->validMoves)
        return p2->validMoves - p1->validMoves;
    if (p1->invalidMoves != p2->invalidMoves)    
        return p2->invalidMoves - p1->invalidMoves;
    return strcmp(p1->name, p2->name);        

}




int main(int argc, char * argv[]) {

    // manejo de parámetros (asumo buen uso de master.c)
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    // conectar a mem compartida
    const char *GAME_STATE_SHM = "/game_state";
    const char *GAME_SYNC_SHM = "/game_sync";
    size_t boardSize = width * height * sizeof(int);
    size_t stateSize = sizeof(GameState) + boardSize;
    size_t syncSize = sizeof(GameSync);

    GameState * state = createSHM(GAME_STATE_SHM, stateSize);
    GameSync * sync = createSHM(GAME_SYNC_SHM, syncSize);
    

    while(!state->isGameOver)
    {
        // esperar aviso de master.c
        sem_wait(&sync->view_reading_pending);

        // TODO BORRAR, esto es para testear acceso a memoria compartida
        printf("Número de jugadores: %u\n", state->numPlayers);
        printf("Juego terminado: %d\n", state->isGameOver);
        printf("width:%u x height%u\n", width, height);

        // este me sirve de debug de player.c
        printf("###  validMoves P1: %d    ###\n", state->players[0].validMoves);
        printf("###  invalidMoves P1: %d  ###\n", state->players[0].invalidMoves);
        printf("\n");

        printTableContent(state, width, height); // TODO, esto lo de pasar todo el state puede estar mal

        
        Player * leaderboard[state->numPlayers];
        // Se llena el leaderboard con ids de jugadores
        for (int i=0; i < state->numPlayers; i++) {                 
            leaderboard[i] = &state->players[i];                                     
        }

        qsort(leaderboard, state->numPlayers, sizeof(Player *), comparePlayersPositions);



        // Print leaderboard
        for (int i = 1; i <= state->numPlayers; i++) {
            // int numPlayer = leaderboard[i-1];
            // Player player = state->players[numPlayer];

            Player * currPlayer = leaderboard[i - 1];


            // TODO esto es ineficiente, porque se podría guardar en leaderboard tipo un struct {Player, numPlayer}
            int numPlayer;
            for(int j = 0; j < state->numPlayers; j++)
            {
                if(currPlayer->pid == state->players[j].pid)
                {
                    numPlayer = j;
                }
            }


            printf("%s%d°. %s%s%s %d %d %d\n", (8+i <= 11) ? colors[8+i] : RESET, i, colors[numPlayer], currPlayer->name, RESET, currPlayer->score, currPlayer->validMoves, currPlayer->invalidMoves);
        }

        sem_post(&sync->view_writing_done);
    }
    

    sem_post(&sync->view_writing_done); // TODO este tal vez está de más

    
    // desconectar de mem compartida
    closeSHM(state, stateSize);
    closeSHM(sync, syncSize);


    return 0;
    
}
