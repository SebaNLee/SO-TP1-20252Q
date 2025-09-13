#include "view.h"
#include "sync.h"

#define RESET "\033[0m"
#define SIGN_BG "\033[48;2;150;75;50m"
#define SIGN "\033[48;2;184;122;92m"
#define GRASS "\033[48;2;28;134;58m"
#define WATER "\033[48;2;58;131;218m"

#define RED "\033[48;2;199;44;38m"
#define CYAN "\033[48;2;0;255;255m"
#define GREEN "\033[48;2;0;204;122m"
#define YELLOW "\033[48;2;229;214;47m"
#define PINK "\033[48;2;207;99;188m"
#define ORANGE "\033[48;2;255;165;0m"
#define WHITE "\033[48;2;212;212;212m"
#define BLACK "\033[48;2;0;0;0m"
#define PURPLE "\033[48;2;146;36;142m"

#define GOLD_BG "\033[48;2;212;175;55m"
#define SILVER_BG "\033[48;2;192;192;192m"
#define BRONZE_BG "\033[48;2;205;127;50m"

#define TAB_NUMBER 8
#define TIMES_TAB(width) (width / 3)

const char *colors[] = {RED, CYAN, GREEN, YELLOW, PINK, ORANGE, WHITE, PURPLE, BLACK, GOLD_BG, SILVER_BG, BRONZE_BG};

void fillSpacesWithColor(int spaces, const char *color)
{
    for (int i = 0; i < spaces; i++)
    {
        printf("%s ", color);
    }
}

void printLineColor(int width, const char *color)
{
    tabulate(width, color);
    for (int i = 0; i < 4 * width + 1; i++)
    {
        printf(" ");
    }
    tabulate(width, color);
}

void printTitle(int width)
{
    printLineColor(width, SIGN_BG);
    newLine();
    int centerWidth = 2 * 8 * (int)TIMES_TAB(width) - 4 + 4 * width + 1;
    printf("%s  ", SIGN_BG);
    fillSpacesWithColor(centerWidth, SIGN);
    printf("%s  ", SIGN_BG);
    newLine();

    char *title = "🏆 QUACKCHAMPS🏆";
    int visualLength = strlen(title) - 2 * 4 + 2 * 2;
    int leftSpaces = (centerWidth - visualLength) / 2;
    int rightSpaces = centerWidth - visualLength - leftSpaces;

    printf("%s  ", SIGN_BG);
    fillSpacesWithColor(leftSpaces, SIGN);
    printf("%s", title);
    fillSpacesWithColor(rightSpaces, SIGN);
    printf("%s  ", SIGN_BG);
    newLine();

    printf("%s  ", SIGN_BG);
    fillSpacesWithColor(centerWidth, SIGN);
    printf("%s  ", SIGN_BG);
    newLine();
    printLineColor(width, SIGN_BG);
}

void tabulate(int width, const char *color)
{
    for (int i = 0; i < TIMES_TAB(width); i++)
    {
        fillSpacesWithColor(TAB_NUMBER, color);
    }
}

void newLine()
{
    printf("%s\x1b[K\n", RESET);
}

void printHeader(int columns, int rows)
{
    int counter = 1;
    printf("%s\n", GRASS);
    tabulate(columns, GRASS);
    printf("%s╔", WATER);
    while (counter++ < columns)
    {
        printf("═══╦");
    }
    printf("═══╗%s", GRASS);
    tabulate(columns, GRASS);
    if (rows == 10)
    {
        fillSpacesWithColor(TAB_NUMBER, RESET);
        printf("%s        LEADERBOARD       | Pnt | VMo | IMo |", SIGN);
    }
    newLine();
}

void printTableContent(GameState *state, int width, int height, Player **leaderboard)
{

    int numPlayers = state->numPlayers;
    int *board = state->board;
    Player *players = state->players;
    int numPlayersProcessed = (height == 10) ? 0 : numPlayers + 1;

    printTitle(width);
    int topPadding = (height - state->numPlayers) / 2;
    printHeader(width, height);

    for (int i = 0; i < height; i++)
    {
        tabulate(width, GRASS);
        printf("%s", WATER);
        for (int j = 0; j < width; j++)
        {

            int val = board[i * width + j];
            char isHead = 0;
            for (int p = 0; p < numPlayers; p++)
            {
                if (players[p].x == j && players[p].y == i)
                    isHead = 1;
            }

            // por PVS Studio
            int playerIndex = abs(val);
            if (playerIndex >= sizeof(colors) / sizeof(colors[0]))
            {
                perror("Se re fue playerIndex en view.c");
                exit(1);
            }

            if (val <= 0)
            {
                if (isHead)
                {
                    printf("║%s🦆 %s", colors[playerIndex], WATER);
                }
                else
                {
                    printf("║%s   %s", colors[playerIndex], WATER);
                }
            }
            else
            {
                printf("║ %d ", val);
            }
        }
        printf("║%s", GRASS);
        tabulate(width, GRASS);
        if (i - topPadding == 0 && height != 10)
        {
            numPlayersProcessed = -1;
        }
        if (numPlayersProcessed < numPlayers)
        {
            if (numPlayersProcessed < 0)
            {
                fillSpacesWithColor(TAB_NUMBER, RESET);
                printf("%s        LEADERBOARD       | Pnt | VMo | IMo |", SIGN);
            }
            else
            {
                fillSpacesWithColor(TAB_NUMBER, RESET);
                int numPlayer;
                for (int j = 0; j < state->numPlayers; j++)
                {
                    if (leaderboard[numPlayersProcessed]->pid == state->players[j].pid)
                    {
                        numPlayer = j;
                    }
                }
                printf("%s|%s  %d°. %s|%s %s", SIGN, (9 + numPlayersProcessed <= 11) ? colors[9 + numPlayersProcessed] : RESET, numPlayersProcessed + 1, SIGN,
                       colors[numPlayer], leaderboard[numPlayersProcessed]->name);
                fillSpacesWithColor(17 - strlen(leaderboard[numPlayersProcessed]->name), colors[numPlayer]);
                printf("| %d%s |", leaderboard[numPlayersProcessed]->score, (leaderboard[numPlayersProcessed]->score < 100) ? ((leaderboard[numPlayersProcessed]->score < 10) ? "  " : " ") : "");
                printf(" %d%s |", leaderboard[numPlayersProcessed]->validMoves, (leaderboard[numPlayersProcessed]->validMoves < 100) ? ((leaderboard[numPlayersProcessed]->validMoves < 10) ? "  " : " ") : "");
                printf(" %d%s %s|", leaderboard[numPlayersProcessed]->invalidMoves, (leaderboard[numPlayersProcessed]->invalidMoves < 100) ? ((leaderboard[numPlayersProcessed]->invalidMoves < 10) ? "  " : " ") : "", SIGN);
            }
        }

        newLine();

        if (i + 1 < state->height)
        {
            printf("%s", GRASS);
            tabulate(width, GRASS);
            printf("%s╠═══", WATER);
            for (int j = 1; j < width; j++)
            {
                printf("╬═══");
            }
            printf("╣%s", GRASS);
            tabulate(width, GRASS);
            if (numPlayersProcessed++ < numPlayers)
            {
                fillSpacesWithColor(TAB_NUMBER, RESET);
                printf("%s---------------------------------------------", SIGN);
            }
            newLine();
        }
    }

    printBase(width);
}

void printBase(int columns)
{
    int counter = 1;
    printf("%s", GRASS);
    tabulate(columns, GRASS);
    printf("%s╚", WATER);
    while (counter++ < columns)
    {
        printf("═══╩");
    }
    printf("═══╝%s", GRASS);
    tabulate(columns, GRASS);
    newLine();
}

int comparePlayersPositions(const void *a, const void *b)
{

    const Player *p1 = *(const Player **)a;
    const Player *p2 = *(const Player **)b;

    if (p1->score != p2->score)
        return p2->score - p1->score;
    if (p1->validMoves != p2->validMoves)
        return p1->validMoves - p2->validMoves;
    if (p1->invalidMoves != p2->invalidMoves)
        return p1->invalidMoves - p2->invalidMoves;
    return strcmp(p1->name, p2->name);
}

bool isDraw(Player *p1, Player *p2)
{

    return (p1->score == p2->score) && (p1->validMoves == p2->validMoves) && (p1->invalidMoves == p2->invalidMoves);
}

int main(int argc, char *argv[])
{

    // manejo de parámetros (asumo buen uso de master.c)
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    // conectar a mem compartida
    const char *GAME_STATE_SHM = "/game_state";
    const char *GAME_SYNC_SHM = "/game_sync";
    size_t boardSize = width * height * sizeof(int);
    size_t stateSize = sizeof(GameState) + boardSize;
    size_t syncSize = sizeof(GameSync);

    GameState *state = (GameState *)createSHM(GAME_STATE_SHM, stateSize, false, false);
    GameSync *sync = (GameSync *)createSHM(GAME_SYNC_SHM, syncSize, false, true);

    while (!state->isGameOver)
    {
        // esperar aviso de master.c
        viewWaitSync(sync);

        printf("\033[H");     // Mover el cursor al principio, para sobreescribir sobre el tablero antiguo
        printf("\n");

        Player *leaderboard[state->numPlayers];
        // Se llena el leaderboard con ids de jugadores
        for (int i = 0; i < state->numPlayers; i++)
        {
            leaderboard[i] = &state->players[i];
        }

        qsort(leaderboard, state->numPlayers, sizeof(Player *), comparePlayersPositions);

        printTableContent(state, width, height, leaderboard);

        viewPostSync(sync);
    }

    newLine();
    newLine();
    Player *leaderboard[state->numPlayers];
    // Se llena el leaderboard con ids de jugadores
    for (int i = 0; i < state->numPlayers; i++)
    {
        leaderboard[i] = &state->players[i];
    }

    qsort(leaderboard, state->numPlayers, sizeof(Player *), comparePlayersPositions);

    const char *color1;
    const char *color2;
    const char *color3;

    for (int i = 0; i < state->numPlayers; i++)
    {
        if (state->players[i].pid == leaderboard[0]->pid)
            color1 = colors[i];
        else if (state->players[i].pid == leaderboard[1]->pid && state->numPlayers > 1)
            color2 = colors[i];
        else if (state->players[i].pid == leaderboard[2]->pid && state->numPlayers > 2)
            color3 = colors[i];
    }

    if (!isDraw(leaderboard[0], leaderboard[1]))
    {
        for (int i = 0; i < 7; i++)
        {
            tabulate(width, RESET);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(17, (state->numPlayers > 1) ? color2 : RESET);
            fillSpacesWithColor(2, RESET), fillSpacesWithColor(17, color1);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(17, (state->numPlayers > 2) ? color3 : RESET);
            newLine();
        }

        for (int i = 8; i > 0; i--)
        {
            tabulate(width, RESET);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(i, (state->numPlayers > 1) ? color2 : RESET);
            fillSpacesWithColor(17 - 2 * i, RESET);
            fillSpacesWithColor(i, (state->numPlayers > 1) ? color2 : RESET);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(i, color1);
            fillSpacesWithColor(17 - 2 * i, RESET);
            fillSpacesWithColor(i, color1);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(i, (state->numPlayers > 2) ? color3 : RESET);
            fillSpacesWithColor(17 - 2 * i, RESET);
            fillSpacesWithColor(i, (state->numPlayers > 2) ? color3 : RESET);
            fillSpacesWithColor(2, RESET);
            newLine();
        }

        tabulate(width, RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17 + 1, RESET);
        fillSpacesWithColor(8, RESET);
        printf("🦆");
        fillSpacesWithColor(8, RESET);
        fillSpacesWithColor(17 + 2, RESET);
        newLine();

        for (int i = 0; i < 3; i++)
        {
            tabulate(width, RESET);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(17 + 2, RESET);
            fillSpacesWithColor(17, GOLD_BG);
            fillSpacesWithColor(17 + 2, RESET);
            newLine();
        }

        tabulate(width, RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(7, RESET);
        if (state->numPlayers > 1)
        {
            printf("🦆");
        }
        else
        {
            printf("  ");
        }
        fillSpacesWithColor(8 + 2, RESET);
        fillSpacesWithColor(17, GOLD_BG);
        fillSpacesWithColor(17 + 2, RESET);
        newLine();

        for (int i = 0; i < 3; i++)
        {
            tabulate(width, RESET);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(17, (state->numPlayers > 1) ? SILVER_BG : RESET);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(17, GOLD_BG);
            fillSpacesWithColor(17 + 2, RESET);
            newLine();
        }

        tabulate(width, RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17, (state->numPlayers > 1) ? SILVER_BG : RESET);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(17, GOLD_BG);
        fillSpacesWithColor(2, RESET);
        fillSpacesWithColor(7, RESET);
        if (state->numPlayers > 2)
        {
            printf("🦆");
        }
        else
        {
            printf("  ");
        }
        fillSpacesWithColor(8, RESET);
        newLine();

        for (int i = 0; i < 5; i++)
        {
            tabulate(width, RESET);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(17, (state->numPlayers > 1) ? SILVER_BG : RESET);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(17, GOLD_BG);
            fillSpacesWithColor(2, RESET);
            fillSpacesWithColor(17, (state->numPlayers > 2) ? BRONZE_BG : RESET);
            newLine();
        }

        tabulate(width, RESET);
        int padding, left, right;
        fillSpacesWithColor(2, RESET);
        if (state->numPlayers > 1)
        {
            padding = 17 - strlen(leaderboard[1]->name);
            left = padding / 2;
            right = padding - left;
            fillSpacesWithColor(left, RESET);
            printf("%s", leaderboard[1]->name);
            fillSpacesWithColor(right, RESET);
        }
        else
        {
            fillSpacesWithColor(17, RESET);
        }
        fillSpacesWithColor(2, RESET);
        padding = 17 - strlen(leaderboard[0]->name);
        left = padding / 2;
        right = padding - left;
        fillSpacesWithColor(left, RESET);
        printf("%s", leaderboard[0]->name);
        fillSpacesWithColor(right, RESET);
        fillSpacesWithColor(2, RESET);
        if (state->numPlayers > 2)
        {
            padding = 17 - strlen(leaderboard[2]->name);
            left = padding / 2;
            right = padding - left;
            fillSpacesWithColor(left, RESET);
            printf("%s", leaderboard[2]->name);
            fillSpacesWithColor(right, RESET);
            fillSpacesWithColor(2, RESET);
        }
        newLine();
    }
    else
    {
        printf("%s%s🦆 %s 🤝  %s🦆 %s%s\n", color1, leaderboard[0]->name, RESET, color2, leaderboard[1]->name, RESET);
    }

    // desconectar de mem compartida
    closeSHM(state, stateSize);
    closeSHM(sync, syncSize);

    return 0;
}
