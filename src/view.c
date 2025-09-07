#include "view.h"

#define RESET   "\033[0m"
#define RED     "\033[41m"
#define GREEN   "\033[42m"
#define YELLOW  "\033[43m"
#define BLUE    "\033[44m"
#define MAGENTA "\033[45m"

#define WHITE   "\033[47m"
#define BRIGHT_BLUE     "\033[104m"
#define BRIGHT_MAGENTA   "\033[105m"
#define ORANGE  "\033[48;2;255;165;0m"
#define BLACK   "\033[48;2;0;0;0m"
#define CYAN    "\033[48;2;0;255;255m"
#define GOLD_BG   "\033[48;2;212;175;55m"   
#define SILVER_BG "\033[48;2;192;192;192m"  
#define BRONZE_BG "\033[48;2;205;127;50m"  
#define BROWN_BG   "\033[48;2;184;122;92m"
#define DARK_BROWN "\033[48;2;150;75;50m"
#define GRASS       "\033[48;2;28;134;58m"
#define TAB_NUMBER  8
#define TIMES_TAB(width) (width/3) 


const char* colors[] = {RED, CYAN, GREEN, YELLOW, MAGENTA, ORANGE, WHITE, BRIGHT_MAGENTA, BLACK, GOLD_BG, SILVER_BG, BRONZE_BG};

void fillSpacesWithColor(int spaces, const char * color) {
    for (int i = 0; i < spaces; i++) {
        printf("%s ", color);
    }
}

void printLineColor(int width, const char * color) {
    tabulate(width, color);
    for (int i = 0; i < 4 * width + 1; i++) {
        printf(" ");
    }
    tabulate(width, color);
    
}

void printTitle(int width) {
    printLineColor(width, DARK_BROWN); newLine();
    int centerWidth = 2 * 8 * (int)TIMES_TAB(width) - 4 + 4*width + 1;
    printf("%s  ", DARK_BROWN); fillSpacesWithColor(centerWidth, BROWN_BG); printf("%s  ", DARK_BROWN); newLine();

    char * title = "🏆 QUACKCHAMPS🏆";
    int visualLength = strlen(title) - 2*4 + 2*2; 
    int leftSpaces  = (centerWidth - visualLength) / 2;
    int rightSpaces = centerWidth - visualLength - leftSpaces;

    printf("%s  ", DARK_BROWN);
    fillSpacesWithColor(leftSpaces, BROWN_BG);
    printf("%s", title);
    fillSpacesWithColor(rightSpaces, BROWN_BG);
    printf("%s  ", DARK_BROWN); newLine();

    printf("%s  ", DARK_BROWN); fillSpacesWithColor(centerWidth, BROWN_BG); printf("%s  ", DARK_BROWN); newLine();
    printLineColor(width, DARK_BROWN); 
    
}

void tabulate(int width, const char * color) {
    for (int i = 0; i < TIMES_TAB(width); i++) {
        fillSpacesWithColor(TAB_NUMBER, color);
    }
}

void newLine() {
    printf("%s\x1b[K\n", RESET);
}

void printHeader(int columns, int rows) {
        int counter = 1;
        printf("%s\n", GRASS);
        tabulate(columns, GRASS);
        printf("%s╔", BRIGHT_BLUE);
        while (counter++ < columns) {
            printf("═══╦");
        }
        printf("═══╗%s", GRASS);
        tabulate(columns, GRASS);
        if (rows == 10) {
            fillSpacesWithColor(TAB_NUMBER, RESET);
            printf("%s     LEADERBOARD    |  PT | VM | IM |", BROWN_BG);
        }
        newLine();

}

void printTableContent(GameState * state, int width, int height, Player ** leaderboard) {

    

    int numPlayers = state->numPlayers;
    int * board = state->board;
    Player * players = state->players;
    int numPlayersProcessed = (height==10) ? 0 : numPlayers+1;
    
    int ScreenWidth = 2 * width/3 + 4* width + 1;  

    printTitle(width);
    int topPadding = (height - state->numPlayers) / 2;
    printHeader(width, height);
    

    for (int i = 0; i < height; i++) {
        tabulate(width, GRASS);
        printf("%s", BRIGHT_BLUE);
        for (int j = 0; j < width; j++) {
            
            int val = board[i * width + j];
            char isHead = 0;
            for (int p = 0; p < numPlayers; p++) {
                if (players[p].x == j && players[p].y == i)
                    isHead = 1;
            }

            if(val <= 0) {
                if (isHead) {
                    printf("║%s🦆 %s", colors[abs(val)], BRIGHT_BLUE);
                } else {
                    printf("║%s   %s", colors[abs(val)], BRIGHT_BLUE); 
                }
            } else {
                printf("║ %d ", val);
            }

            
        }
        printf("║%s", GRASS);
        tabulate(width, GRASS);
        if (i - topPadding == 0 && height != 10) {
            numPlayersProcessed = -1;
        }
        if (numPlayersProcessed < numPlayers) {
            if (numPlayersProcessed < 0) {
                fillSpacesWithColor(TAB_NUMBER, RESET);
                printf("%s     LEADERBOARD    |  PT | VM | IM |", BROWN_BG);
            } else {
                fillSpacesWithColor(TAB_NUMBER, RESET);
                int numPlayer;
                for(int j = 0; j < state->numPlayers; j++)
                {
                    if(leaderboard[numPlayersProcessed]->pid == state->players[j].pid)
                    {   
                        numPlayer = j;
                    }
                }
                printf("%s|%s  %d°. %s|%s %s", BROWN_BG, (9+numPlayersProcessed <= 11) ? colors[9+numPlayersProcessed] : RESET, numPlayersProcessed+1, BROWN_BG,
                colors[numPlayer], leaderboard[numPlayersProcessed]->name);
                fillSpacesWithColor(11 - strlen(leaderboard[numPlayersProcessed]->name), colors[numPlayer]);
                printf("| %d%s |", leaderboard[numPlayersProcessed]->score, (leaderboard[numPlayersProcessed]->score < 100) ? 
                                   ((leaderboard[numPlayersProcessed]->score < 10) ?  "  " : " ") : "");
                printf(" %d%s |", leaderboard[numPlayersProcessed]->validMoves, (leaderboard[numPlayersProcessed]->validMoves < 10) ? " " : "" );
                printf(" %d%s %s|", leaderboard[numPlayersProcessed]->invalidMoves, (leaderboard[numPlayersProcessed]->invalidMoves < 10) ? " " : "", BROWN_BG);
            }
            
        }

        newLine();

        if (i+1 < state->height) {
            printf("%s", GRASS);
            tabulate(width, GRASS);
            printf("%s╠═══", BRIGHT_BLUE);
            for (int j = 1; j < width; j++) {
                printf("╬═══");
            }
            printf("╣%s", GRASS);
            tabulate(width, GRASS);
            if (numPlayersProcessed++ < numPlayers) {
                fillSpacesWithColor(TAB_NUMBER, RESET);
                printf("%s-------------------------------------", BROWN_BG);    
            }
            newLine();
        } 
    }

    printBase(width);
}

void printBase(int columns) {
    int counter = 1;
        printf("%s", GRASS);
        tabulate(columns, GRASS);
        printf("%s╚", BRIGHT_BLUE);
        while (counter++ < columns) {
            printf("═══╩");
        }
        printf("═══╝%s", GRASS);
        tabulate(columns, GRASS);
        newLine();
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

    GameState * state = createSHM(GAME_STATE_SHM, stateSize, false, false);
    GameSync * sync = createSHM(GAME_SYNC_SHM, syncSize, false, true);
    

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

        Player * leaderboard[state->numPlayers];
        // Se llena el leaderboard con ids de jugadores
        for (int i=0; i < state->numPlayers; i++) {                 
            leaderboard[i] = &state->players[i];                                     
        }

        qsort(leaderboard, state->numPlayers, sizeof(Player *), comparePlayersPositions);

        printTableContent(state, width, height, leaderboard); // TODO, esto lo de pasar todo el state puede estar mal



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

    // Final leaderboard. TODO, que lo haga el master esto de ordenar jugadores
    Player * leaderboard[state->numPlayers];
    // Se llena el leaderboard con ids de jugadores
    for (int i=0; i < state->numPlayers; i++) {                 
        leaderboard[i] = &state->players[i];                                     
    }

    qsort(leaderboard, state->numPlayers, sizeof(Player *), comparePlayersPositions);

    const char * color1; 
    const char * color2;
    const char * color3;

    
    for (int i = 0; i < state->numPlayers; i++) {
        if (state->players[i].pid == leaderboard[0]->pid)
            color1 = colors[i];
        else if (state->players[i].pid == leaderboard[1]->pid && state->numPlayers > 1)  
            color2 = colors[i];
        else if (state->players[i].pid == leaderboard[2]->pid && state->numPlayers > 2)       
            color3 = colors[i];
    }

    for (int i = 0; i < 7; i++) {
        tabulate(width, RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17, (state->numPlayers > 1) ? color2 : RESET);
        fillSpacesWithColor(2, RESET), fillSpacesWithColor(17, color1); fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17, (state->numPlayers > 2) ? color3 : RESET);
        newLine();
    }

    for (int i = 8; i > 0; i--) {
        tabulate(width, RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(i, (state->numPlayers > 1) ? color2 : RESET); fillSpacesWithColor(17-2*i, RESET); fillSpacesWithColor(i, (state->numPlayers > 1) ? color2 : RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(i, color1); fillSpacesWithColor(17-2*i, RESET); fillSpacesWithColor(i, color1); fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(i, (state->numPlayers > 2) ? color3 : RESET); fillSpacesWithColor(17-2*i, RESET); fillSpacesWithColor(i, (state->numPlayers > 2) ? color3 : RESET);
        fillSpacesWithColor(2, RESET);
        newLine();
    }

    tabulate(width, RESET);
    fillSpacesWithColor(2, RESET);
    fillSpacesWithColor(17+1, RESET); fillSpacesWithColor(8, RESET); printf("🦆"); fillSpacesWithColor(8, RESET); fillSpacesWithColor(17+2, RESET); newLine();

    for (int i = 0; i < 3; i++) {
        tabulate(width, RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17+2, RESET); fillSpacesWithColor(17, GOLD_BG); fillSpacesWithColor(17+2, RESET); newLine();
    }

    tabulate(width, RESET);
    fillSpacesWithColor(2, RESET);
    fillSpacesWithColor(7, RESET); 
    if (state->numPlayers > 1) {
        printf("🦆");
    } else {
        printf("  ");
    }
    fillSpacesWithColor(8+2, RESET); fillSpacesWithColor(17, GOLD_BG); fillSpacesWithColor(17+2, RESET); newLine();

    for (int i = 0; i < 3; i++) {
        tabulate(width, RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17, (state->numPlayers > 1) ? SILVER_BG : RESET); fillSpacesWithColor(2, RESET); fillSpacesWithColor(17, GOLD_BG); fillSpacesWithColor(17+2, RESET); newLine();
    }

    tabulate(width, RESET);
    fillSpacesWithColor(2, RESET);
    fillSpacesWithColor(17, (state->numPlayers > 1) ? SILVER_BG : RESET); fillSpacesWithColor(2, RESET); fillSpacesWithColor(17, GOLD_BG); fillSpacesWithColor(2, RESET);
    fillSpacesWithColor(7, RESET); 
    if (state->numPlayers > 2) {
        printf("🦆");
    } else {
        printf("  ");
    }
     fillSpacesWithColor(8, RESET); newLine();

    for (int i = 0; i < 5; i++) {
        tabulate(width, RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17, (state->numPlayers > 1) ? SILVER_BG : RESET); fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17, GOLD_BG); fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17, (state->numPlayers > 2) ? BRONZE_BG : RESET); newLine();
    }

    tabulate(width, RESET);
    int padding, left, right;  
    fillSpacesWithColor(2, RESET);
    if (state->numPlayers > 1) {
        padding = 17 - strlen(leaderboard[1]->name);
        left = padding / 2;
        right = padding - left; 
        fillSpacesWithColor(left, RESET); printf("%s", leaderboard[1]->name); fillSpacesWithColor(right, RESET); 
    } else {
        fillSpacesWithColor(17, RESET);
    }
    fillSpacesWithColor(2, RESET);
    padding = 17 - strlen(leaderboard[0]->name);
    left = padding / 2;
    right = padding - left;
    fillSpacesWithColor(left, RESET); printf("%s", leaderboard[0]->name); fillSpacesWithColor(right, RESET); fillSpacesWithColor(2, RESET);
    if (state->numPlayers > 2) {
        padding = 17 - strlen(leaderboard[2]->name);
        left = padding / 2;
        right = padding - left;
        fillSpacesWithColor(left, RESET); printf("%s", leaderboard[2]->name); fillSpacesWithColor(right, RESET); fillSpacesWithColor(2, RESET); 
    }
    newLine();
    // desconectar de mem compartida
    closeSHM(state, stateSize);
    closeSHM(sync, syncSize);


    return 0;
    
}
