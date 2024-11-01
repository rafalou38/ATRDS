#include "grid.h"
#include "enemies.h"

void allocateGridCells(Grid *grid)
{
    struct Cell **cells;
    cells = (struct Cell **)malloc(sizeof(struct Cell *) * grid->width); // Première coordonnée: x / width(largeur)
    grid->cells = cells;

    if (cells == NULL)
    {
        printCritical("Failed to allocate grid");
        exit(EXIT_FAILURE);
    }
    for (int x = 0; x < grid->width; x++)
    {

        cells[x] = (struct Cell *)malloc(sizeof(struct Cell) * grid->height); // deuxième coordonnée: y / height(hauteur)
        if (cells[x] == NULL)
        {
            printCritical("Failed to allocate grid col");
            exit(EXIT_FAILURE);
        }

        for (int y = 0; y < grid->height; y++)
        {
            cells[x][y].x = x;
            cells[x][y].y = y;
            cells[x][y].type = TERRAIN;
            cells[x][y].visited = false;
            cells[x][y].selected = false;
            cells[x][y].hasTurret = false;
            cells[x][y].drawn = false;
        }
    }
}

void freeGrid(Grid grid)
{
    printf(COLOR_GRAY);
    printf(" $ ");
    printf(RESET);
    printf("Freeing %s%dx%d%s grid:\t", COLOR_YELLOW, grid.width, grid.height, RESET);

    for (int x = 0; x < grid.width; x++)
    {
        free(grid.cells[x]);
    }
    free(grid.cells);

    printf("%s Done %s\n", COLOR_GREEN, RESET);
}

void genBasicPath(Grid *grid)

{
    int chemin_x = 1;
    int chemin_y = (rand() % (grid->height));
    if (chemin_y == grid->height - 1) // Rectifications du random pour pas la 1ere ni la derniere ligne
        chemin_y = chemin_y - 1;

    if (chemin_y == 0)
        chemin_y = chemin_y + 1;

    struct Cell **cells = grid->cells;

    grid->start_x = 0;
    grid->start_y = chemin_y;

    cells[0][chemin_y].type = CHEMIN; // 2 premieres cases sans les autres options, fixées à une hauteur aléatoire
    cells[chemin_x][chemin_y].type = CHEMIN;

    int **historique;
    historique = (int **)malloc(HISTORY_SIZE * (sizeof(int *)));
    for (size_t i = 0; i < HISTORY_SIZE; i++)
    {
        historique[i] = (int *)malloc(2 * sizeof(int));
    }

    int history_index = 0;

    while (chemin_x != grid->width - 1)
    {
        bool posible_bas = chemin_y < grid->height - 2                         //
                           && cells[chemin_x - 1][chemin_y + 1].type != CHEMIN // bas gauche
                           && cells[chemin_x + 1][chemin_y + 1].type != CHEMIN // bas droite
                           && cells[chemin_x][chemin_y + 1].type != CHEMIN     // bas
                           && cells[chemin_x - 1][chemin_y + 2].type != CHEMIN // bas bas gauche
                           && cells[chemin_x][chemin_y + 2].type != CHEMIN     // bas bas
                           && !cells[chemin_x][chemin_y + 1].visited;

        bool possible_haut = chemin_y > 1                                        //
                             && cells[chemin_x - 1][chemin_y - 1].type != CHEMIN // haut gauche
                             && cells[chemin_x + 1][chemin_y - 1].type != CHEMIN // haut droit
                             && cells[chemin_x][chemin_y - 1].type != CHEMIN     // haut
                             && cells[chemin_x][chemin_y - 2].type != CHEMIN     // haut haut
                             && cells[chemin_x - 1][chemin_y - 2].type != CHEMIN // haut haut gauche
                             && !cells[chemin_x][chemin_y - 1].visited;

        bool possible_droite = cells[chemin_x + 1][chemin_y + 1].type != CHEMIN                                 // droite bas
                               && cells[chemin_x + 1][chemin_y - 1].type != CHEMIN                              // droite haut
                               && cells[chemin_x + 1][chemin_y].type != CHEMIN                                  // droite
                               && (chemin_x == grid->width - 2 || cells[chemin_x + 2][chemin_y].type != CHEMIN) // droite droite
                               && !cells[chemin_x + 1][chemin_y].visited;

        bool possible_gauche = chemin_x > 1                                        //
                               && cells[chemin_x - 1][chemin_y + 1].type != CHEMIN // gauche bas
                               && cells[chemin_x - 1][chemin_y - 1].type != CHEMIN // gauche haut
                               && cells[chemin_x - 1][chemin_y].type != CHEMIN     // gauche
                               && cells[chemin_x - 2][chemin_y].type != CHEMIN     // gauche gauche
                               && cells[chemin_x - 2][chemin_y + 1].type != CHEMIN // gauche gauche haut
                               && cells[chemin_x - 2][chemin_y - 1].type != CHEMIN // gauche gauche bas
                               && !cells[chemin_x - 1][chemin_y].visited;

        if (!posible_bas && !possible_haut && !possible_droite && !possible_gauche)
        {
            cells[chemin_x][chemin_y].type = TERRAIN;
            history_index -= 1;
            chemin_x = historique[history_index][0];
            chemin_y = historique[history_index][1];
            cells[chemin_x][chemin_y].index = history_index;
            continue;
        }

        while (true)
        {
            int random_chemin = rand() % 7;
            if (random_chemin <= 1 && posible_bas)
            {
                chemin_y++;
                break;
            }
            else if (random_chemin <= 3 && possible_haut)
            {
                chemin_y--;
                break;
            }
            else if (random_chemin <= 5 && possible_gauche)
            {
                chemin_x--;
                break;
            }
            else if (random_chemin == 6 && possible_droite)
            {
                chemin_x++;
                break;
            }
        }

        history_index++;
        assert(history_index < HISTORY_SIZE);

        historique[history_index][0] = chemin_x;
        historique[history_index][1] = chemin_y;

        cells[chemin_x][chemin_y].type = CHEMIN;
        cells[chemin_x][chemin_y].visited = true;
        cells[chemin_x][chemin_y].index = history_index;
    }

    grid->end_x = chemin_x;
    grid->end_y = chemin_y;

    for (size_t i = 0; i < HISTORY_SIZE; i++)
    {
        free(historique[i]);
    }
    free(historique);
}
void fillBG(int xmin, int ymin, int xmax, int ymax)
{
    printf(COLOR_STANDARD_BG);
    for (int x = xmin; x < xmax; x++)
    {
        for (int y = ymin; y < ymax; y++)
        {
            move_to(x, y);
            printf(" ");
        }
    }
}
void drawCell(struct Cell cell, Grid grid)
{
    if (cell.type == TERRAIN)
    {

        for (int y = 0; y < CELL_HEIGHT; y++)
        {

            int terminal_x = cell.x * (CELL_WIDTH + GAP) + 3;
            int terminal_y = cell.y * (CELL_HEIGHT + GAP / 2) + 2;

            printf(COLOR_TOWER_SLOT_BG);
            move_to(terminal_x, terminal_y + y);

            {
                for (int x = 0; x < CELL_WIDTH; x++)
                {
                    printf(" ");
                }
            }

            if (cell.selected)
            {
                printf(COLOR_SELECTED_SLOT);
            }
            move_to(terminal_x, terminal_y + y);
            for (int x = 0; x < CELL_WIDTH; x++)
            {
                if (cell.selected)
                {
                    if (x == 0)
                    {
                        printf("█");
                    }
                    else if (x == CELL_WIDTH - 1)
                    {
                        move_to(terminal_x + x, terminal_y + y);
                        printf("█");
                    }
                    else if (y == 0)
                        printf("▀");
                    else if (y == CELL_HEIGHT - 1)
                        printf("▄");
                }
            }
            printf(RESET);
        }
        if (cell.hasTurret)
        {
            int x_current_turret = cell.x * (CELL_WIDTH + GAP) + 3 + 2;
            int y_current_turret = cell.y * (CELL_HEIGHT + GAP / 2) + 2 + 1;

            if (cell.turret.type == Sniper)
            {
                printf(COLOR_SNIPER_BASE);
                char *sprite[2][8][7] = {
                    {
                        {"     ▄██▄     ",
                         "   ▄██████▄   ",
                         " ▄████▀▀████▄ ",
                         COLOR_SNIPER_HEAD "<" COLOR_SNIPER_CANNON "===" COLOR_SNIPER_BASE "█ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █████",
                         " ▀████▄▄████▀ ",
                         "   ▀██████▀   ",
                         "     ▀██▀     "},
                        {"     ▄██▄     ",
                         "   ▄██████▄   ",
                         " ▄████▀▀████▄ ",
                         "█████ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █████",
                         " ▀██" COLOR_SNIPER_CANNON "//" COLOR_SNIPER_BASE "▄▄████▀ ",
                         COLOR_SNIPER_HEAD "  ▕▁▁" COLOR_SNIPER_BASE "█████▀   ",
                         "     ▀██▀     "},
                        {"     ▄██▄     ",
                         "   ▄██████▄   ",
                         " ▄████▀▀████▄ ",
                         "█████ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █████",
                         " ▀████▄▄████▀ ",
                         "   ▀██" COLOR_SNIPER_CANNON "||" COLOR_SNIPER_BASE "██▀   ",
                         "     ▀" COLOR_SNIPER_HEAD "\\/" COLOR_SNIPER_BASE "▀     "},
                        {"     ▄██▄     ",
                         "   ▄██████▄   ",
                         " ▄████▀▀████▄ ",
                         "█████ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █████",
                         " ▀████▄▄" COLOR_SNIPER_CANNON "\\\\" COLOR_SNIPER_BASE "██▀ ",
                         "   ▀█████" COLOR_SNIPER_HEAD "▁▁▏  " COLOR_SNIPER_BASE,
                         "     ▀██▀     "},
                        {"     ▄██▄     ",
                         "   ▄██████▄   ",
                         " ▄████▀▀████▄ ",
                         "█████ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █" COLOR_SNIPER_CANNON "===" COLOR_SNIPER_HEAD ">" COLOR_SNIPER_BASE,
                         " ▀████▄▄████▀ ",
                         "   ▀██████▀   ",
                         "     ▀██▀     "},
                        {"     ▄██▄     ",
                         "   ▄█████" COLOR_SNIPER_HEAD "▔▔▏  " COLOR_SNIPER_BASE,
                         " ▄████▀▀" COLOR_SNIPER_CANNON "//" COLOR_SNIPER_BASE "██▄ ",
                         "█████ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █████",
                         " ▀████▄▄████▀ ",
                         "   ▀██████▀   ",
                         "     ▀██▀     "},
                        {"     ▄" COLOR_SNIPER_HEAD "/\\" COLOR_SNIPER_BASE "▄     ",
                         "   ▄██" COLOR_SNIPER_CANNON "||" COLOR_SNIPER_BASE "██▄   ",
                         " ▄████▀▀████▄ ",
                         "█████ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █████",
                         " ▀████▄▄████▀ ",
                         "   ▀██████▀   ",
                         "     ▀██▀     "},
                        {"     ▄██▄     ",
                         COLOR_SNIPER_HEAD "  ▕▔▔" COLOR_SNIPER_BASE "█████▄   ",
                         " ▄██" COLOR_SNIPER_CANNON "\\\\" COLOR_SNIPER_BASE "▀▀████▄ ",
                         "█████ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █████",
                         " ▀████▄▄████▀ ",
                         "   ▀██████▀   ",
                         "     ▀██▀     "},
                    },
                    {
                        {" ▄▄  ▄██▄  ▄▄ ",
                         " ▀▀▄██▀▀██▄▀▀ ",
                         " ▄████▀▀████▄ ",
                         COLOR_SNIPER_HEAD "⊃" COLOR_SNIPER_CANNON " ==" COLOR_SNIPER_BASE "█ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █ ███",
                         " ▀████▄▄████▀ ",
                         " ▄▄▀██▄▄██▀▄▄ ",
                         " ▀▀  ▀██▀  ▀▀ "},
                        {" ▄▄  ▄██▄  ▄▄ ",
                         " ▀▀▄██▀▀██▄▀▀ ",
                         " ▄████▀▀████▄ ",
                         "███▄█ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █ ███",
                         " ▀██" COLOR_SNIPER_CANNON "//" COLOR_SNIPER_BASE "▄▄████▀ ",
                         COLOR_SNIPER_HEAD "   ⊃ " COLOR_SNIPER_BASE "██▄██▀▄▄ ",
                         "     ▀██▀  ▀▀ "},
                        {" ▄▄  ▄██▄  ▄▄ ",
                         " ▀▀▄██▀▀██▄▀▀ ",
                         " ▄████▀▀████▄ ",
                         "███ █ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █ ███",
                         " ▀████▄▄████▀ ",
                         " ▄▄▀██" COLOR_SNIPER_CANNON "||" COLOR_SNIPER_BASE "██▀▄▄ ",
                         " ▀▀  ▀" COLOR_SNIPER_HEAD "⎾⏋" COLOR_SNIPER_BASE "▀  ▀▀ "},
                        {" ▄▄  ▄██▄  ▄▄ ",
                         " ▀▀▄██▀▀██▄▀▀ ",
                         " ▄████▀▀████▄ ",
                         "███ █ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █▄███",
                         " ▀████▄▄" COLOR_SNIPER_CANNON "\\\\" COLOR_SNIPER_BASE "██▀ ",
                         " ▄▄▀██▄██" COLOR_SNIPER_HEAD "⊂    " COLOR_SNIPER_BASE,
                         " ▀▀  ▀██▀     "},
                        {" ▄▄  ▄██▄  ▄▄ ",
                         " ▀▀▄██▀▀██▄▀▀ ",
                         " ▄████▀▀████▄ ",
                         "███ █ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █" COLOR_SNIPER_CANNON "===" COLOR_SNIPER_HEAD "⊂" COLOR_SNIPER_BASE,
                         " ▀████▄▄████▀ ",
                         " ▄▄▀██▄▄██▀▄▄ ",
                         " ▀▀  ▀██▀  ▀▀ "},
                        {" ▄▄  ▄██▄     ",
                         " ▀▀▄██▀██" COLOR_SNIPER_HEAD "⊂    " COLOR_SNIPER_BASE,
                         " ▄████▀▀" COLOR_SNIPER_CANNON "//" COLOR_SNIPER_BASE "██▄ ",
                         "███ █ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █▀███",
                         " ▀████▄▄████▀ ",
                         " ▄▄▀██▄▄██▀▄▄ ",
                         " ▀▀  ▀██▀  ▀▀ "},
                        {" ▄▄  ▄" COLOR_SNIPER_HEAD "⎿⏌" COLOR_SNIPER_BASE "▄  ▄▄ ",
                         " ▀▀▄██" COLOR_SNIPER_CANNON "||" COLOR_SNIPER_BASE "██▄▀▀ ",
                         " ▄████▀▀████▄ ",
                         "███ █ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █ ███",
                         " ▀████▄▄████▀ ",
                         " ▄▄▀██▄▄██▀▄▄ ",
                         " ▀▀  ▀██▀  ▀▀ "},
                        {"     ▄██▄  ▄▄ ",
                         COLOR_SNIPER_HEAD "   ⊃ " COLOR_SNIPER_BASE "██▀██▄▀▀ ",
                         " ▄██" COLOR_SNIPER_CANNON "\\\\" COLOR_SNIPER_BASE "▀▀████▄ ",
                         "███▀█ " COLOR_SNIPER_CANNON "[]" COLOR_SNIPER_BASE " █ ███",
                         " ▀████▄▄████▀ ",
                         " ▄▄▀██▄▄██▀▄▄ ",
                         " ▀▀  ▀██▀  ▀▀ "},
                    }};

                // angle est approché au PI/le plus proche sous forme d'index
                float angle = atan2f(-cell.turret.last_shot_dy, cell.turret.last_shot_dx);
                size_t index = MIN(round(((angle + PI) / (2 * PI)) * 8), 7);
                assert(index < 8);

                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][index][i]);
                }

                printf(RESET);
            }
            else if (cell.turret.type == Inferno)
            {
                printf(COLOR_INFERNO_BASE);

                char *sprite[2][7] = {
                    {"     ▄" COLOR_INFERNO_HEAD "/\\" COLOR_INFERNO_BASE "▄     ",
                     "  ▄███" COLOR_INFERNO_CANNON "||" COLOR_INFERNO_BASE "███▄  ",
                     " ▄██▄█" COLOR_INFERNO_CANNON "||" COLOR_INFERNO_BASE "█▄██▄ ",
                     COLOR_INFERNO_HEAD "<" COLOR_INFERNO_CANNON "=====[]=====" COLOR_INFERNO_HEAD ">" COLOR_INFERNO_BASE,
                     " ▀██▀█" COLOR_INFERNO_CANNON "||" COLOR_INFERNO_BASE "█▀██▀ ",
                     "  ▀███" COLOR_INFERNO_CANNON "||" COLOR_INFERNO_BASE "███▀  ",
                     "     ▀" COLOR_INFERNO_HEAD "\\/" COLOR_INFERNO_BASE "▀     "},

                    {"     ▄" COLOR_INFERNO_HEAD "/\\" COLOR_INFERNO_BASE "▄     ",
                     COLOR_INFERNO_HEAD "  ⇖ " COLOR_INFERNO_BASE "▀█" COLOR_INFERNO_CANNON "||" COLOR_INFERNO_BASE "█▀" COLOR_INFERNO_HEAD "⇗   ",
                     COLOR_INFERNO_BASE " ▄█▄" COLOR_INFERNO_CANNON "⇖ ||⇗ " COLOR_INFERNO_BASE "▄█▄ ",
                     COLOR_INFERNO_HEAD "<"
                                        "=====[]=====" COLOR_INFERNO_HEAD ">",
                     COLOR_INFERNO_BASE " ▀█▀" COLOR_INFERNO_CANNON "⇙ ||⇘ " COLOR_INFERNO_BASE "▀█▀ ",
                     COLOR_INFERNO_HEAD "  ⇙ " COLOR_INFERNO_BASE "▄█" COLOR_INFERNO_CANNON "||" COLOR_INFERNO_BASE "█▄" COLOR_INFERNO_HEAD "⇘   ",
                     COLOR_INFERNO_BASE "     ▀" COLOR_INFERNO_HEAD "\\/" COLOR_INFERNO_BASE "▀     "}};

                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][i]);
                }

                printf(RESET);
            }
            else if (cell.turret.type == Mortier)
            {
                printf(COLOR_MORTIER_BASE);

                char *sprite[2][2][7] = {
                    {{"  ██████████  ",
                      "██ ▄██████▄ ██",
                      "████▀    ▀████",
                      "████      ████",
                      "████▄    ▄████",
                      "██ ▀██████▀ ██",
                      "  ██████████  "},
                     {"  ██████████  ",
                      "██ ▄█▀▀▀▀█▄ ██",
                      "███▀ " COLOR_MORTIER_FIRING "▄██▄" COLOR_MORTIER_BASE " ▀███",
                      "███ " COLOR_MORTIER_FIRING "██" COLOR_MORTIER_FIRING_CENTER "██" COLOR_MORTIER_FIRING "██" COLOR_MORTIER_BASE " ███",
                      "███▄ " COLOR_MORTIER_FIRING "▀██▀" COLOR_MORTIER_BASE " ▄███",
                      "██ ▀█▄▄▄▄█▀ ██",
                      "  ██████████  "}},
                    {{"▄█▀▀▀▀██▀▀▀▀█▄",
                      "█  ▄██████▄  █",
                      "█ ██▀    ▀██ █",
                      "████      ████",
                      "█ ██▄    ▄██ █",
                      "█  ▀██████▀  █",
                      "▀█▄▄▄▄██▄▄▄▄█▀"},
                     {"▄█▀▀▀▀██▀▀▀▀█▄",
                      "█  ▄██▄▄██▄  █",
                      "█ ██▀ " COLOR_MORTIER_FIRING "▄▄" COLOR_MORTIER_BASE " ▀██ █",
                      "██ █ " COLOR_MORTIER_FIRING "█" COLOR_MORTIER_FIRING_CENTER "██" COLOR_MORTIER_FIRING "█" COLOR_MORTIER_BASE " █ ██",
                      "█ ██▄ " COLOR_MORTIER_FIRING "▀▀" COLOR_MORTIER_BASE " ▄██ █",
                      "█  ▀██▀▀██▀  █",
                      "▀█▄▄▄▄██▄▄▄▄█▀"}}};
                int mortier_shooting;
                if (cell.turret.compteur < 0.5)
                {
                    mortier_shooting = 1;
                }
                else
                {
                    mortier_shooting = 0;
                }
                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][mortier_shooting][i]);
                }

                printf(RESET);
            }
            else if (cell.turret.type == Gatling)
            {
                printf(COLOR_GATLING_BASE);

                char *sprite[2][5][7] = {
                    {{"  ████▀▀████  ",
                      "██  ██▄▄██  ██",
                      "████▀ "COLOR_GATLING_COOLING"▄▄"COLOR_GATLING_BASE" ▀████",
                      "█  █ "COLOR_GATLING_COOLING"█  █"COLOR_GATLING_BASE" █  █",
                      "████▄ "COLOR_GATLING_COOLING"▀▀"COLOR_GATLING_BASE" ▄████",
                      "██  ██▀▀██  ██",
                      "  ████▄▄████  "},
                     {"  ████▀▀████  ",
                      "██  ██▄▄██  ██",
                      "█████▀  ▀█████",
                      "█  █  "COLOR_GATLING_FIRING"██"COLOR_GATLING_BASE"  █  █",
                      "█████▄  ▄█████",
                      "██  ██▀▀██  ██",
                      "  ████▄▄████  "},
                     {"  █████▀▀███  ",
                      "███  ██▄▄█▀▀██",
                      "█▀▀██▀  ▀█▄▄██",
                      "█▄▄█  "COLOR_GATLING_FIRING"██"COLOR_GATLING_BASE"  █▀▀█",
                      "██▀▀█▄  ▄██▄▄█",
                      "██▄▄█▀▀██  ███",
                      "  ███▄▄█████  "},
                     {"  ██▀▀██▀▀██  ",
                      "████▄▄██▄▄████",
                      "█  ██▀  ▀██  █",
                      "████  "COLOR_GATLING_FIRING"██"COLOR_GATLING_BASE"  ████",
                      "█  ██▄  ▄██  █",
                      "████▀▀██▀▀████",
                      "  ██▄▄██▄▄██  "},
                     {"  ███▀▀█████  ",
                      "██▀▀█▄▄██  ███",
                      "██▄▄█▀  ▀██▀▀█",
                      "█▀▀█  "COLOR_GATLING_FIRING"██"COLOR_GATLING_BASE"  █▄▄█",
                      "█▄▄██▄  ▄█▀▀██",
                      "███  ██▀▀█▄▄██",
                      "  █████▄▄███  "}},
                    {{"  ▄▄██▀▀██▄▄  ",
                      " █  ██▄▄██  █ ",
                      "████▀ "COLOR_GATLING_COOLING"▄▄"COLOR_GATLING_BASE" ▀████",
                      "█  █ "COLOR_GATLING_COOLING"█  █"COLOR_GATLING_BASE" █  █",
                      "████▄ "COLOR_GATLING_COOLING"▀▀"COLOR_GATLING_BASE" ▄████",
                      " █  ██▀▀██  █ ",
                      "  ▀▀██▄▄██▀▀  "},
                     {"  ▄▄██▀▀██▄▄  ",
                      " █  ██▄▄██  █ ",
                      "█████▀  ▀█████",
                      "█  █  "COLOR_GATLING_FIRING"██"COLOR_GATLING_BASE"  █  █",
                      "█████▄  ▄█████",
                      " █  ██▀▀██  █ ",
                      "  ▀▀██▄▄██▀▀  "},
                     {"  ▄▄███▀▀█▄▄  ",
                      " ██  ██▄▄█▀▀█ ",
                      "█▀▀██▀  ▀█▄▄██",
                      "█▄▄█  "COLOR_GATLING_FIRING"██"COLOR_GATLING_BASE"  █▀▀█",
                      "██▀▀█▄  ▄██▄▄█",
                      " █▄▄█▀▀██  ██ ",
                      "  ▀▀█▄▄███▀▀  "},
                     {"  ▄▄▀▀██▀▀▄▄  ",
                      " ███▄▄██▄▄███ ",
                      "█  ██▀  ▀██  █",
                      "████  "COLOR_GATLING_FIRING"██"COLOR_GATLING_BASE"  ████",
                      "█  ██▄  ▄██  █",
                      " ███▀▀██▀▀███ ",
                      "  ▀▀▄▄██▄▄▀▀  "},
                     {"  ▄▄█▀▀███▄▄  ",
                      " █▀▀█▄▄██  ██ ",
                      "██▄▄█▀  ▀██▀▀█",
                      "█▀▀█  "COLOR_GATLING_FIRING"██"COLOR_GATLING_BASE"  █▄▄█",
                      "█▄▄██▄  ▄█▀▀██",
                      " ██  ██▀▀█▄▄█ ",
                      "  ▀▀███▄▄█▀▀  "}}};
                int gatling_shooting;
                gatling_shooting = floor(cell.turret.compteur*(4/cell.turret.reload_delay[cell.turret.lvl])+1);
                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][gatling_shooting][i]);
                }
            }
            else if (cell.turret.type == Freezer)
            {
                printf(COLOR_FREEZER_BASE);

                char *sprite[2][2][7] = {
                    {{"   ▄  ██  ▄   ",
                      " ▄  ██▀▀██  ▄ ",
                      "  ██▀    ▀██  ",
                      "███        ███",
                      "  ██▄    ▄██  ",
                      " ▀  ██▄▄██  ▀ ",
                      "   ▀  ██  ▀   "},
                     {"   ▄  ██  ▄   ",
                      " ▄  ██▀▀██  ▄ ",
                      "  ██▀ " COLOR_FREEZER_FIRING "▄▄" COLOR_FREEZER_BASE " ▀██  ",
                      "███  " COLOR_FREEZER_FIRING "█" COLOR_FREEZER_FIRING_CENTER "██" COLOR_FREEZER_FIRING "█" COLOR_FREEZER_BASE "  ███",
                      "  ██▄ " COLOR_FREEZER_FIRING "▀▀" COLOR_FREEZER_BASE " ▄██  ",
                      " ▀  ██▄▄██  ▀ ",
                      "   ▀  ██  ▀   "}},
                    {{"  ▀▄ ▄▀▀▄ ▄▀  ",
                      "▀▄ ▄██▀▀██▄ ▄▀",
                      " ▄██▄▀▀▀▀▄██▄ ",
                      "█ █ █    █ █ █",
                      " ▀██▀▄▄▄▄▀██▀ ",
                      "▄▀ ▀██▄▄██▀ ▀▄",
                      "  ▄▀ ▀▄▄▀ ▀▄  "},
                     {"  ▀▄ ▄▀▀▄ ▄▀  ",
                      "▀▄ ▄██▀▀██▄ ▄▀",
                      " ▄██  " COLOR_FREEZER_FIRING "▄▄" COLOR_FREEZER_BASE "  ██▄ ",
                      "█ █  " COLOR_FREEZER_FIRING "█" COLOR_FREEZER_FIRING_CENTER "██" COLOR_FREEZER_FIRING "█" COLOR_FREEZER_BASE "  █ █",
                      " ▀██  " COLOR_FREEZER_FIRING "▀▀" COLOR_FREEZER_BASE "  ██▀ ",
                      "▄▀ ▀██▄▄██▀ ▀▄",
                      "  ▄▀ ▀▄▄▀ ▀▄  "}}};
                int freezer_shooting;
                if (cell.turret.compteur < 0.5)
                {
                    freezer_shooting = 1;
                }
                else
                {
                    freezer_shooting = 0;
                }
                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][freezer_shooting][i]);
                }
            }
            else if (cell.turret.type == Petrificateur)
            {
                printf("Tornade");
            }
            else if (cell.turret.type == Banque)
            {
                printf(COLOR_BANQUE_BASE);

                char *sprite[2][10][7] = {
                    {{"    ▄▄▄▄▄▄    ",
                      " ▄███▄▄▄▄███▄ ",
                      " ███" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_BASE "██▄▄█ ",
                      " ███" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_BASE "██▄▄█ ",
                      " ██▀▀▀▀▀██▀▀█ ",
                      " ████████████ ",
                      "              "},
                     {"    ▄▄▄▄▄▄    ",
                      " ▄███▄▄▄▄███▄ ",
                      " ███" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_BASE "██▄▄█ ",
                      " ███" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_BASE "██▄▄█ ",
                      " ██▀▀▀▀▀██▀▀█ ",
                      " ████████████ ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "              ",
                      "              ",
                      "      ██      ",
                      "              ",
                      "              ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "              ",
                      "     ▄▄▄▄     ",
                      "     █  █     ",
                      "     ▀▀▀▀     ",
                      "              ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "              ",
                      "    █▀▀▀▀█    ",
                      "    █+10 █    ",
                      "    █▄▄▄▄█    ",
                      "              ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "   ▄▄▄▄▄▄▄▄   ",
                      "   █      █   ",
                      "   █ +10  █   ",
                      "   █      █   ",
                      "   ▀▀▀▀▀▀▀▀   ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "  █▀▀▀▀▀▀▀▀█  ",
                      "  █        █  ",
                      "  █  +10   █  ",
                      "  █        █  ",
                      "  █▄▄▄▄▄▄▄▄█  ",
                      "              "},
                     {COLOR_BANQUE_MONEY " ▄▄▄▄▄▄▄▄▄▄▄▄ ",
                      " █          █ ",
                      " █    ▄▜ ▛▜ █ ",
                      " █ ▟▙  ▐ ▌▐ █ ",
                      " █ ▜▛  ▐ ▙▟ █ ",
                      " █          █ ",
                      " ▀▀▀▀▀▀▀▀▀▀▀▀ "},
                     {COLOR_BANQUE_MONEY "█▀▀▀▀▀▀▀▀▀▀▀▀█",
                      "█            █",
                      "█     ▄▜ ▛▜  █",
                      "█  ▟▙  ▐ ▌▐  █",
                      "█  ▜▛  ▐ ▙▟  █",
                      "█            █",
                      "█▄▄▄▄▄▄▄▄▄▄▄▄█"},
                     {COLOR_BANQUE_GENERATION "█▀▀▀▀▀▀▀▀▀▀▀▀█",
                      "█            █",
                      "█     ▄▜ ▛▜  █",
                      "█  ▟▙  ▐ ▌▐  █",
                      "█  ▜▛  ▐ ▙▟  █",
                      "█            █",
                      "█▄▄▄▄▄▄▄▄▄▄▄▄█"}},
                    {{" ▄▄▄▄▄▄▄ ██ ██",
                      "██████████████",
                      "███" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_BASE "███",
                      "███" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_BASE "███",
                      "█▀▀▀██▀▀▀▀██▀█",
                      "█▀██▀▀▀██▀▀▀▀█",
                      "▀▀▀▀▀▀▀▀▀▀▀▀▀▀"},
                     {" ▄▄▄▄▄▄▄ ██ ██",
                      "██████████████",
                      "███" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_BASE "███",
                      "███" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_GENERATION "$" COLOR_BANQUE_MONEY "$" COLOR_BANQUE_BASE "███",
                      "█▀▀▀██▀▀▀▀██▀█",
                      "█▀██▀▀▀██▀▀▀▀█",
                      "▀▀▀▀▀▀▀▀▀▀▀▀▀▀"},
                     {COLOR_BANQUE_MONEY "              ",
                      "              ",
                      "              ",
                      "      ██      ",
                      "              ",
                      "              ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "              ",
                      "     ▄▄▄▄     ",
                      "     █  █     ",
                      "     ▀▀▀▀     ",
                      "              ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "              ",
                      "    █▀▀▀▀█    ",
                      "    █+30 █    ",
                      "    █▄▄▄▄█    ",
                      "              ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "   ▄▄▄▄▄▄▄▄   ",
                      "   █      █   ",
                      "   █ +30  █   ",
                      "   █      █   ",
                      "   ▀▀▀▀▀▀▀▀   ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "  █▀▀▀▀▀▀▀▀█  ",
                      "  █        █  ",
                      "  █  +30   █  ",
                      "  █        █  ",
                      "  █▄▄▄▄▄▄▄▄█  ",
                      "              "},
                     {COLOR_BANQUE_MONEY " ▄▄▄▄▄▄▄▄▄▄▄▄ ",
                      " █          █ ",
                      " █    ▀▜ ▛▜ █ ",
                      " █ ▟▙ ▄▟ ▌▐ █ ",
                      " █ ▜▛ ▄▟ ▙▟ █ ",
                      " █          █ ",
                      " ▀▀▀▀▀▀▀▀▀▀▀▀ "},
                     {COLOR_BANQUE_MONEY "█▀▀▀▀▀▀▀▀▀▀▀▀█",
                      "█            █",
                      "█     ▀▜ ▛▜  █",
                      "█  ▟▙ ▄▟ ▌▐  █",
                      "█  ▜▛ ▄▟ ▙▟  █",
                      "█            █",
                      "█▄▄▄▄▄▄▄▄▄▄▄▄█"},
                     {COLOR_BANQUE_GENERATION "█▀▀▀▀▀▀▀▀▀▀▀▀█",
                      "█            █",
                      "█     ▀▜ ▛▜  █",
                      "█  ▟▙ ▄▟ ▌▐  █",
                      "█  ▜▛ ▄▟ ▙▟  █",
                      "█            █",
                      "█▄▄▄▄▄▄▄▄▄▄▄▄█"}}};
                int banque_generating;
                double d = cell.turret.compteur;
                double fractionnaire_d = d - floor(d);
                if (0 < fractionnaire_d && fractionnaire_d < 0.5)
                {
                    banque_generating = 1;
                }
                else
                {
                    banque_generating = 0;
                }
                if (floor(d) == 1)
                {
                    banque_generating = fractionnaire_d * 10;
                }
                int fracd10 = fractionnaire_d * 10;
                if (floor(d) == 2)
                {
                    if (fracd10 % 2 == 1)
                    {
                        banque_generating = 8;
                    }
                    else
                    {
                        banque_generating = 9;
                    }
                }
                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][banque_generating][i]);
                }

                printf(RESET);
            }
        }
    }
    if (cell.type == CHEMIN)
    {
        // Détermination du type de chemin
        bool chemin_vers_haut = cell.y - 1 >= 0 && grid.cells[cell.x][cell.y - 1].type == CHEMIN;
        bool chemin_vers_droite = cell.x + 1 < grid.width && grid.cells[cell.x + 1][cell.y].type == CHEMIN;
        bool chemin_vers_bas = cell.y + 1 < grid.height && grid.cells[cell.x][cell.y + 1].type == CHEMIN;
        bool chemin_vers_gauche = cell.x - 1 >= 0 && grid.cells[cell.x - 1][cell.y].type == CHEMIN;

        bool chemin_vers_haut_gauche = cell.x - 1 >= 0 && cell.y - 1 >= 0 && grid.cells[cell.x - 1][cell.y - 1].type == CHEMIN;
        bool chemin_vers_haut_droit = cell.x + 1 < grid.width && cell.y - 1 >= 0 && grid.cells[cell.x + 1][cell.y - 1].type == CHEMIN;
        bool chemin_vers_bas_droit = cell.x + 1 < grid.width && cell.y + 1 < grid.height && grid.cells[cell.x + 1][cell.y + 1].type == CHEMIN;
        bool chemin_vers_bas_gauche = cell.x - 1 >= 0 && cell.y + 1 < grid.height && grid.cells[cell.x - 1][cell.y + 1].type == CHEMIN;

        printf(COLOR_STANDARD_BG);
        printf(COLOR_PATH_BORDER);
        for (int y = 0; y < CELL_HEIGHT + 2; y++)
        {
            int terminal_x = (cell.x * (CELL_WIDTH + GAP) + 3);
            int terminal_y = (cell.y * (CELL_HEIGHT + GAP / 2) + 2);

            move_to(terminal_x - 1, terminal_y + y - 1);

            for (int x = 0; x < CELL_WIDTH + 2; x++)
            {
                bool side = (cell.x == 0 && x == 0) || (cell.x == grid.width - 1 && x == CELL_WIDTH + 1);
                if (
                    !side && ((x == 0 && y == 0 && !chemin_vers_gauche && !chemin_vers_haut)                                            //
                              || (x == CELL_WIDTH + 1 && y == CELL_HEIGHT + 1 && chemin_vers_bas && chemin_vers_droite)                 //
                              || (x == CELL_WIDTH + 1 && y == 0 && chemin_vers_haut_droit && chemin_vers_haut && !chemin_vers_droite))) //
                    printf("┏");
                // printf("╭");
                //
                else if (!side && ((x == CELL_WIDTH + 1 && y == 0 && !chemin_vers_droite && !chemin_vers_haut)  //
                                   || (x == 0 && y == CELL_HEIGHT + 1 && chemin_vers_gauche && chemin_vers_bas) //
                                   || (x == 0 && y == 0 && chemin_vers_haut && chemin_vers_haut_gauche && !chemin_vers_gauche)))
                    printf("┓");
                // printf("╮");
                //
                else if (!side                                                                                        //
                         && ((x == CELL_WIDTH + 1 && y == CELL_HEIGHT + 1 && !chemin_vers_droite && !chemin_vers_bas) //
                             || (x == 0 && y == 0 && chemin_vers_haut && chemin_vers_gauche && !chemin_vers_haut_gauche) || (x == 0 && y == CELL_HEIGHT + 1 && chemin_vers_bas && !chemin_vers_gauche && chemin_vers_bas_gauche)))
                    printf("┛");
                // printf("╯");
                //
                else if (
                    !side                                                                                                       //
                    && ((x == 0 && y == CELL_HEIGHT + 1 && !chemin_vers_gauche && !chemin_vers_bas)                             //
                        || (y == 0 && x == CELL_WIDTH + 1 && chemin_vers_haut && chemin_vers_droite && !chemin_vers_haut_droit) //
                        || (y == CELL_HEIGHT + 1 && x == CELL_WIDTH + 1 && chemin_vers_bas && !chemin_vers_droite && chemin_vers_bas_droit)))
                    printf("┗");
                // printf("╰");
                //
                else if (!(                                                        //
                             (cell.x == 0 && x == 0)                               //
                             || (cell.x == grid.width - 1 && x == CELL_WIDTH + 1)) //
                         && ((x == 0 && !chemin_vers_gauche)                       //
                             || (x == CELL_WIDTH + 1 && !chemin_vers_droite)))
                    printf("┃");
                // printf("│");
                //
                else if (((y == 0 && !chemin_vers_haut) //
                          || (y == CELL_HEIGHT + 1 && !chemin_vers_bas)))
                    printf("━");
                // printf("─");
                //
                else if (x >= 1 && x <= CELL_WIDTH && y >= 1 && y <= CELL_HEIGHT)
                {
                    printf(" ");
                }
                else
                {
                    printf(" ");
                }
            }
        }
        printf(RESET);
    }
}

void drawFullGrid(Grid grid)
{

    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++)
        {
            drawCell(grid.cells[x][y], grid);
        }
    }
}
void clearPath(Grid grid)
{
    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++)
        {
            if (grid.cells[x][y].type == CHEMIN)
                drawCell(grid.cells[x][y], grid);
        }
    }
}

void clearUsedPath(Grid grid, EnemyPool ep)
{
    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++)
        {
            if (grid.cells[x][y].drawn)
                continue;
            // A coté de tourelle
            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (x > 0 && y > 0 && x < grid.width - 1 && y < grid.height - 1 && grid.cells[x + dx][y + dy].hasTurret)
                    {
                        drawCell(grid.cells[x][y], grid);

                        grid.cells[x][y].drawn = true;
                    }
                }
            }

            // Chemin occupé
            if (grid.cells[x][y].type == CHEMIN)
            {
                for (int i = 0; i < ep.count; i++)
                {
                    if (
                        (ep.enemies[i].previous_cell.x == x && ep.enemies[i].previous_cell.y == y) //
                        || (ep.enemies[i].previous_cell.x + 1 == x && ep.enemies[i].previous_cell.y == y))
                    {
                        drawCell(grid.cells[x][y], grid);
                        grid.cells[x][y].drawn = true;
                        break;
                    }
                }
            }
            else if (grid.cells[x][y].hasTurret == true)
            {
                drawCell(grid.cells[x][y], grid);
                grid.cells[x][y].drawn = true;
            }
        }
    }
    // Reset
    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++)
        {
            grid.cells[x][y].drawn = false;
        }
    }
}