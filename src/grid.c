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

        for (int y = 0; y < grid->height; y++) // Initialisation d'un terrain vierge de x*y
        {
            cells[x][y].x = x;
            cells[x][y].y = y;
            cells[x][y].type = TERRAIN;
            cells[x][y].visited = false;
            cells[x][y].selected = false;
            cells[x][y].hasTurret = false;
            cells[x][y].drawn = false;
            cells[x][y].has_effect = false;
            cells[x][y].effect = 0;
            cells[x][y].effect_counter = -1;
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

void genBasicPath(Grid *grid) // Génération du terrain par rapport a la seed (rand())

{
    int chemin_x = 1;
    int chemin_y = (rand() % (grid->height));
    if (chemin_y == grid->height - 1) // Rectifications du random pour ne pas être sur les extrémités hautes et basses
        chemin_y = chemin_y - 1;

    if (chemin_y == 0)
        chemin_y = chemin_y + 1;

    struct Cell **cells = grid->cells;

    grid->start_x = 0;
    grid->start_y = chemin_y;

    cells[0][chemin_y].type = CHEMIN; // Les 2 premiers chemins sont à la même hauteur fixée aléatoirement
    cells[1][chemin_y].type = CHEMIN;

    int **historique; // Création d'un historique du chemin qui permet de ne pas tourner en boucle lors de sa génération
    historique = (int **)malloc(HISTORY_SIZE * (sizeof(int *)));
    if (historique == NULL)
    {
        printCritical("Failed to allocate historique");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < HISTORY_SIZE; i++)
    {
        historique[i] = (int *)malloc(2 * sizeof(int));
        if (historique[i] == NULL)
        {
            printCritical("Failed to allocate an indice of historique");
            exit(EXIT_FAILURE);
        }
    }

    int history_index = 0;

    while (chemin_x != grid->width - 1) // Conditions pour la création d'une case chemin (POur ne pas que les chemins soient collés)
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

        if (!posible_bas && !possible_haut && !possible_droite && !possible_gauche) // Si aucun chemin n'est possible, on remonte dans l'historique jusqu'à ce qu'il le soit
        {
            cells[chemin_x][chemin_y].type = TERRAIN;
            history_index -= 1;
            chemin_x = historique[history_index][0];
            chemin_y = historique[history_index][1];
            cells[chemin_x][chemin_y].index = history_index;
            continue;
        }

        while (true) // Application des chemins
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
        // Mise à jour de l'historique
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

    for (size_t i = 0; i < HISTORY_SIZE; i++) // Libération de l'historique à la fin de la création de chemin
    {
        free(historique[i]);
    }
    free(historique);
}
void fillBG(int xmin, int ymin, int xmax, int ymax) // Efface tout
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
void drawCell(struct Cell cell, Grid grid) // Dessine les cellules selon leur status
{
    if (cell.type == TERRAIN) // Si la cellule est un terrain
    {
        if (cell.hasTurret == false) // Et qu'elle est sans tourelle, elle met du vide sur toute la case
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
                printf(RESET);
            }
        }
        if (cell.selected) // La cellule selectionnée est dessinée avec un cadre autour
        {
            int terminal_x = cell.x * (CELL_WIDTH + GAP) + 3;
            int terminal_y = cell.y * (CELL_HEIGHT + GAP / 2) + 2;
            printf(COLOR_SELECTED_SLOT);
            printf(COLOR_TOWER_SLOT_BG);
            for (int y = 0; y < CELL_HEIGHT; y++)
            {
                move_to(terminal_x, terminal_y + y);
                for (int x = 0; x < CELL_WIDTH; x++)
                {
                    if (cell.selected)
                    {
                        if (x == 0)
                        {
                            printf("█");
                        }
                        if (x == 1 && y != 0 && y != CELL_HEIGHT - 1)
                        {
                            printf(" ");
                        }
                        if (x == CELL_WIDTH - 2 && y != 0 && y != CELL_HEIGHT - 1)
                        {
                            move_to(terminal_x + x, terminal_y + y);
                            printf(" ");
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
            }
        }
        if (cell.hasTurret) // Si la cellule est une tourelle, elle va être déssinée selon son sprite, donc ci dessous la liste des sprites
        {
            int x_current_turret = cell.x * (CELL_WIDTH + GAP) + 3 + 2;
            int y_current_turret = cell.y * (CELL_HEIGHT + GAP / 2) + 2 + 1;
            printf(COLOR_TOWER_SLOT_BG);
            if (cell.turret.type == Sniper) // Sniper, 8 sprites pour les 8 directions
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
                        // Sniper de niveau 2 (niveau 1 et le sniper de base est de niveau 0 dans le programme)
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

                // Angle est approché au PI/le plus proche sous forme d'index, qui sert à afficher le bon sprite du sniper par rapport à sa direction
                float angle = atan2f(-cell.turret.last_shot_dy, cell.turret.last_shot_dx);
                size_t index = MIN(round(((angle + PI) / (2 * PI)) * 8), 7);
                assert(index < 8);

                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][index][i]);
                }
            }
            else if (cell.turret.type == Inferno) // Sprites de l'inferno, la plus simple des tourelles, seulement 2 sprites pour le niveau 1 et 2
            {
                printf(COLOR_INFERNO_BASE);

                char *sprite[2][4][7] = {
                    {{"     ▄██▄     ",
                      "  ▄▀█▀▀▀▀█▀▄  ",
                      " ▄█▀ " COLOR_INFERNO_FIRE1 "▄" COLOR_INFERNO_FIRE2 "▀ ▄" COLOR_INFERNO_BASE " ▀█▄ ",
                      "███  " COLOR_INFERNO_FIRE1 "▀▄█" COLOR_INFERNO_FIRE2 " ▄" COLOR_INFERNO_BASE " ███",
                      " ▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀ ",
                      "  ▀▄█▄▄▄▄█▄▀  ",
                      "     ▀██▀     "},
                     {"     ▄██▄     ",
                      "  ▄▀█▀▀▀▀█▀▄  ",
                      " ▄█▀" COLOR_INFERNO_FIRE2 "▄ ▀ ▄" COLOR_INFERNO_BASE " ▀█▄ ",
                      "███ " COLOR_INFERNO_FIRE1 "▀▄" COLOR_INFERNO_FIRE1 "▀" COLOR_INFERNO_FIRE1 "▄ █" COLOR_INFERNO_BASE " ███",
                      " ▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀ ",
                      "  ▀▄█▄▄▄▄█▄▀  ",
                      "     ▀██▀     "},
                     {"     ▄██▄     ",
                      "  ▄▀█▀▀▀▀█▀▄  ",
                      " ▄█▀ " COLOR_INFERNO_FIRE2 "▄ ▀" COLOR_INFERNO_FIRE1 "▄" COLOR_INFERNO_BASE " ▀█▄ ",
                      "███ " COLOR_INFERNO_FIRE2 "▄ " COLOR_INFERNO_FIRE1 "█▄▀" COLOR_INFERNO_BASE "  ███",
                      " ▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀ ",
                      "  ▀▄█▄▄▄▄█▄▀  ",
                      "     ▀██▀     "},
                     {"     █  █     ",
                      "  ▄▀█▀▀▀▀█▀▄  ",
                      "▄▄█▀ " COLOR_INFERNO_FIRE2 "▄ ▀" COLOR_INFERNO_FIRE1 "▄" COLOR_INFERNO_BASE " ▀█▄▄",
                      "  █ " COLOR_INFERNO_FIRE1 "█ ▄" COLOR_INFERNO_FIRE2 "▀" COLOR_INFERNO_FIRE1 "▄█" COLOR_INFERNO_BASE " █  ",
                      "▀▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀▀",
                      "  ▀▄█▄▄▄▄█▄▀  ",
                      "     █  █     "}},
                    {{"█▀▄  ▄▀▀▄  ▄▀█",
                      " ▀▄▀█▀▀▀▀█▀▄▀ ",
                      " ▄█▀ " COLOR_INFERNO_FIRE1 "▄" COLOR_INFERNO_FIRE2 "▀ ▄" COLOR_INFERNO_BASE " ▀█▄ ",
                      "█ █  " COLOR_INFERNO_FIRE1 "▀▄█" COLOR_INFERNO_FIRE2 " ▄" COLOR_INFERNO_BASE " █ █",
                      " ▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀ ",
                      " ▄▀▄█▄▄▄▄█▄▀▄ ",
                      "█▄▀  ▀▄▄▀  ▀▄█"},
                     {"█▀▄  ▄▀▀▄  ▄▀█",
                      " ▀▄▀█▀▀▀▀█▀▄▀ ",
                      " ▄█▀" COLOR_INFERNO_FIRE2 "▄ ▀ ▄" COLOR_INFERNO_BASE " ▀█▄ ",
                      "█ █ " COLOR_INFERNO_FIRE1 "▀▄" COLOR_INFERNO_FIRE1 "▀" COLOR_INFERNO_FIRE1 "▄ █" COLOR_INFERNO_BASE " █ █",
                      " ▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀ ",
                      " ▄▀▄█▄▄▄▄█▄▀▄ ",
                      "█▄▀  ▀▄▄▀  ▀▄█"},
                     {"█▀▄  ▄▀▀▄  ▄▀█",
                      " ▀▄▀█▀▀▀▀█▀▄▀ ",
                      " ▄█▀ " COLOR_INFERNO_FIRE2 "▄ ▀" COLOR_INFERNO_FIRE1 "▄" COLOR_INFERNO_BASE " ▀█▄ ",
                      "█ █ " COLOR_INFERNO_FIRE2 "▄ " COLOR_INFERNO_FIRE1 "█▄▀" COLOR_INFERNO_BASE "  █ █",
                      " ▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀ ",
                      " ▄▀▄█▄▄▄▄█▄▀▄ ",
                      "█▄▀  ▀▄▄▀  ▀▄█"},
                     {"▄▀▄  █  █  ▄▀▄",
                      " ▀▄▀█▀▀▀▀█▀▄▀ ",
                      "▄▄█▀ " COLOR_INFERNO_FIRE2 "▄ ▀" COLOR_INFERNO_FIRE1 "▄" COLOR_INFERNO_BASE " ▀█▄▄",
                      "  █ " COLOR_INFERNO_FIRE1 "█ ▄" COLOR_INFERNO_FIRE2 "▀" COLOR_INFERNO_FIRE1 "▄█" COLOR_INFERNO_BASE " █  ",
                      "▀▀█▄ " COLOR_INFERNO_TORCH "▀██▀" COLOR_INFERNO_BASE " ▄█▀▀",
                      " ▄▀▄█▄▄▄▄█▄▀▄ ",
                      "▀▄▀  █  █  ▀▄▀"}}};
                int inferno_shooting;
                if (cell.turret.compteur < 0.3 && cell.turret.in_range == true)
                {
                    inferno_shooting = 3;
                }
                else
                {
                    inferno_shooting = ((int)floor(cell.turret.compteur * (3 * (3 - cell.turret.lvl) / cell.turret.reload_delay[cell.turret.lvl]))) % 3;
                    if (inferno_shooting > 2)
                    {
                        inferno_shooting = 2;
                    }
                }
                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][inferno_shooting][i]);
                }
            }
            else if (cell.turret.type == Mortier) // Sprites du mortier, 4 sprites pour le mortier normale et le mortier qui tire (de niveau 1 et de niveau 2)
            {
                printf(COLOR_MORTIER_BASE);

                char *sprite[2][3][7] = {
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
                      "  ██████████  "},
                     {"  ██████████  ",
                      "██ ▄██▀▀██▄ ██",
                      "████  " COLOR_MORTIER_COOLING "▄▄" COLOR_MORTIER_BASE "  ████",
                      "███  " COLOR_MORTIER_COOLING "█  █" COLOR_MORTIER_BASE "  ███",
                      "████  " COLOR_MORTIER_COOLING "▀▀" COLOR_MORTIER_BASE "  ████",
                      "██ ▀██▄▄██▀ ██",
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
                      "▀█▄▄▄▄██▄▄▄▄█▀"},
                     {"▄█▀▀▀▀██▀▀▀▀█▄",
                      "█  ▄██▀▀██▄  █",
                      "█ ██  " COLOR_MORTIER_COOLING "▄▄" COLOR_MORTIER_BASE "  ██ █",
                      "███  " COLOR_MORTIER_COOLING "█  █" COLOR_MORTIER_BASE "  ███",
                      "█ ██  " COLOR_MORTIER_COOLING "▀▀" COLOR_MORTIER_BASE "  ██ █",
                      "█  ▀██▄▄██▀  █",
                      "▀█▄▄▄▄██▄▄▄▄█▀"}}};

                int mortier_shooting; // Détermination du shoot du mortier par rapport à son horloge interne

                if (cell.turret.in_range == false)
                {
                    mortier_shooting = 2;
                }
                else if (cell.turret.compteur < 0.5)
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
            }
            else if (cell.turret.type == Gatling) // Sprites de la gatling
            {
                printf(COLOR_GATLING_BASE);

                char *sprite[2][5][7] = {
                    {{"  ████▀▀████  ",
                      "██  ██▄▄██  ██",
                      "█████▀  ▀█████",
                      "█  █  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  █  █",
                      "█████▄  ▄█████",
                      "██  ██▀▀██  ██",
                      "  ████▄▄████  "},
                     {"  █████▀▀███  ",
                      "███  ██▄▄█▀▀██",
                      "█▀▀██▀  ▀█▄▄██",
                      "█▄▄█  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  █▀▀█",
                      "██▀▀█▄  ▄██▄▄█",
                      "██▄▄█▀▀██  ███",
                      "  ███▄▄█████  "},
                     {"  ██▀▀██▀▀██  ",
                      "████▄▄██▄▄████",
                      "█  ██▀  ▀██  █",
                      "████  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  ████",
                      "█  ██▄  ▄██  █",
                      "████▀▀██▀▀████",
                      "  ██▄▄██▄▄██  "},
                     {"  ███▀▀█████  ",
                      "██▀▀█▄▄██  ███",
                      "██▄▄█▀  ▀██▀▀█",
                      "█▀▀█  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  █▄▄█",
                      "█▄▄██▄  ▄█▀▀██",
                      "███  ██▀▀█▄▄██",
                      "  █████▄▄███  "},
                     {"  ████▀▀████  ",
                      "██  ██▄▄██  ██",
                      "████▀ " COLOR_GATLING_COOLING "▄▄" COLOR_GATLING_BASE " ▀████",
                      "█  █ " COLOR_GATLING_COOLING "█  █" COLOR_GATLING_BASE " █  █",
                      "████▄ " COLOR_GATLING_COOLING "▀▀" COLOR_GATLING_BASE " ▄████",
                      "██  ██▀▀██  ██",
                      "  ████▄▄████  "}},
                    {{"  ▄▄██▀▀██▄▄  ",
                      " █  ██▄▄██  █ ",
                      "█████▀  ▀█████",
                      "█  █  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  █  █",
                      "█████▄  ▄█████",
                      " █  ██▀▀██  █ ",
                      "  ▀▀██▄▄██▀▀  "},
                     {"  ▄▄███▀▀█▄▄  ",
                      " ██  ██▄▄█▀▀█ ",
                      "█▀▀██▀  ▀█▄▄██",
                      "█▄▄█  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  █▀▀█",
                      "██▀▀█▄  ▄██▄▄█",
                      " █▄▄█▀▀██  ██ ",
                      "  ▀▀█▄▄███▀▀  "},
                     {"  ▄▄▀▀██▀▀▄▄  ",
                      " ███▄▄██▄▄███ ",
                      "█  ██▀  ▀██  █",
                      "████  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  ████",
                      "█  ██▄  ▄██  █",
                      " ███▀▀██▀▀███ ",
                      "  ▀▀▄▄██▄▄▀▀  "},
                     {"  ▄▄█▀▀███▄▄  ",
                      " █▀▀█▄▄██  ██ ",
                      "██▄▄█▀  ▀██▀▀█",
                      "█▀▀█  " COLOR_GATLING_FIRING "██" COLOR_GATLING_BASE "  █▄▄█",
                      "█▄▄██▄  ▄█▀▀██",
                      " ██  ██▀▀█▄▄█ ",
                      "  ▀▀███▄▄█▀▀  "},
                     {"  ▄▄██▀▀██▄▄  ",
                      " █  ██▄▄██  █ ",
                      "████▀ " COLOR_GATLING_COOLING "▄▄" COLOR_GATLING_BASE " ▀████",
                      "█  █ " COLOR_GATLING_COOLING "█  █" COLOR_GATLING_BASE " █  █",
                      "████▄ " COLOR_GATLING_COOLING "▀▀" COLOR_GATLING_BASE " ▄████",
                      " █  ██▀▀██  █ ",
                      "  ▀▀██▄▄██▀▀  "}}};
                int gatling_shooting; // Calcul du sprite par rapport à l'horloge interne, intervalles reguliers entre chaque sprites
                gatling_shooting = floor(cell.turret.compteur * (4 / cell.turret.reload_delay[cell.turret.lvl]));
                if (gatling_shooting > 3.99)
                {
                    gatling_shooting = 3;
                }
                if (cell.turret.in_range == false)
                {
                    gatling_shooting = 4;
                }
                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][gatling_shooting][i]);
                }
            }
            else if (cell.turret.type == Freezer) // Sprites du freezer, similaire au mortier
            {
                printf(COLOR_FREEZER_BASE);

                char *sprite[2][6][7] = {
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
                      "   ▀  ██  ▀   "},
                     {"      ██      ",
                      " ▄  ██▀▀██  ▄ ",
                      "  ██▀  " COLOR_FREEZER_FIRING_CENTER "▄" COLOR_FREEZER_BASE " ▀██  ",
                      "███  " COLOR_FREEZER_FIRING_CENTER "▀  ▄" COLOR_FREEZER_BASE "  ███",
                      "  ██▄ " COLOR_FREEZER_FIRING_CENTER "▀" COLOR_FREEZER_BASE "  ▄██  ",
                      " ▀  ██▄▄██  ▀ ",
                      "      ██      "},
                     {"      ██      ",
                      " ▄  ██▀▀██  ▄ ",
                      "  ██▀ " COLOR_FREEZER_FIRING_CENTER "▄" COLOR_FREEZER_BASE "  ▀██  ",
                      "███  " COLOR_FREEZER_FIRING_CENTER "▄  ▀" COLOR_FREEZER_BASE "  ███",
                      "  ██▄  " COLOR_FREEZER_FIRING_CENTER "▀" COLOR_FREEZER_BASE " ▄██  ",
                      " ▀  ██▄▄██  ▀ ",
                      "      ██      "},
                     {"      ██      ",
                      " ▄  ██▀▀██  ▄ ",
                      "  ██▀" COLOR_FREEZER_FIRING_CENTER "▄  ▄" COLOR_FREEZER_BASE "▀██  ",
                      "███        ███",
                      "  ██▄" COLOR_FREEZER_FIRING_CENTER "▀  ▀" COLOR_FREEZER_BASE "▄██  ",
                      " ▀  ██▄▄██  ▀ ",
                      "      ██      "},
                     {"      ██      ",
                      " ▄  ██▀▀██  ▄ ",
                      "  ██▀    ▀██  ",
                      "███   " COLOR_FREEZER_FIRING_CENTER "██" COLOR_FREEZER_BASE "   ███",
                      "  ██▄    ▄██  ",
                      " ▀  ██▄▄██  ▀ ",
                      "      ██      "}},
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
                      "  ▄▀ ▀▄▄▀ ▀▄  "},
                     {"     ▄▀▀▄     ",
                      "   ▄██▀▀██▄   ",
                      " ▄██   " COLOR_FREEZER_FIRING_CENTER "▄" COLOR_FREEZER_BASE "  ██▄ ",
                      "█ █  " COLOR_FREEZER_FIRING_CENTER "▀  ▄" COLOR_FREEZER_BASE "  █ █",
                      " ▀██  " COLOR_FREEZER_FIRING_CENTER "▀" COLOR_FREEZER_BASE "   ██▀ ",
                      "   ▀██▄▄██▀   ",
                      "     ▀▄▄▀     "},
                     {"     ▄▀▀▄     ",
                      "   ▄██▀▀██▄   ",
                      " ▄██  " COLOR_FREEZER_FIRING_CENTER "▄" COLOR_FREEZER_BASE "   ██▄ ",
                      "█ █  " COLOR_FREEZER_FIRING_CENTER "▄  ▀" COLOR_FREEZER_BASE "  █ █",
                      " ▀██   " COLOR_FREEZER_FIRING_CENTER "▀" COLOR_FREEZER_BASE "  ██▀ ",
                      "   ▀██▄▄██▀   ",
                      "     ▀▄▄▀     "},
                     {"     ▄▀▀▄     ",
                      "   ▄██▀▀██▄   ",
                      " ▄██ " COLOR_FREEZER_FIRING_CENTER "▄  ▄" COLOR_FREEZER_BASE " ██▄ ",
                      "█ █        █ █",
                      " ▀██ " COLOR_FREEZER_FIRING_CENTER "▀  ▀" COLOR_FREEZER_BASE " ██▀ ",
                      "   ▀██▄▄██▀   ",
                      "     ▀▄▄▀     "},
                     {"     ▄▀▀▄     ",
                      "   ▄██▀▀██▄   ",
                      " ▄██      ██▄ ",
                      "█ █   " COLOR_FREEZER_FIRING_CENTER "██" COLOR_FREEZER_BASE "   █ █",
                      " ▀██      ██▀ ",
                      "   ▀██▄▄██▀   ",
                      "     ▀▄▄▀     "}}};
                int freezer_shooting;
                if (cell.turret.compteur < 0.5 && cell.turret.in_range == true)
                {
                    freezer_shooting = 1;
                }
                else
                {
                    if (cell.turret.in_range == false)
                    {
                        freezer_shooting = floor(cell.turret.compteur * (4 / cell.turret.reload_delay[cell.turret.lvl])) + 2;
                        if (freezer_shooting > 5)
                        {
                            freezer_shooting = 5;
                        }
                    }
                    else
                    {
                        freezer_shooting = 0;
                    }
                }
                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][freezer_shooting][i]);
                }
            }
            else if (cell.turret.type == Petrificateur) // Sprites du pétrificateur (y'en a beaucoup car c'est la seule tourelle avec une animation compliquée)
            {
                if (cell.turret.lvl == 0)
                {
                    printf(COLOR_PETRIFICATEUR_BASE);
                }
                else
                {
                    printf(COLOR_PETRIFICATEUR_BASE_LVL2);
                }

                char *sprite[2][20][7] = {
                    {{"   ▄▄    ▄▄   ",
                      " ▄▀  █▄▄█  ▀▄ ",
                      " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█  █▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "   █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE "  █   ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█  █▀" COLOR_PETRIFICATEUR_BASE "▀▀▄ ",
                      " ▀▄  █▀▀█  ▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀  █  █  ▀▄ ",
                      " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█▀▀█▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ " COLOR_PETRIFICATEUR_FIRING,
                      "    █    █    " COLOR_PETRIFICATEUR_BASE,
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█▄▄█▀" COLOR_PETRIFICATEUR_BASE "▀▀▄ ",
                      " ▀▄  █  █  ▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀  █  ▀  ▀▄ ",
                      " ▀▄ " COLOR_PETRIFICATEUR_FIRING "▄█▀▀█▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ " COLOR_PETRIFICATEUR_FIRING,
                      "    █    █    " COLOR_PETRIFICATEUR_BASE,
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█▄▄█▀" COLOR_PETRIFICATEUR_BASE " ▀▄ ",
                      " ▀▄  ▄  █  ▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄   ▄▄▄   ",
                      " ▄▀  █     ▀▄ ",
                      " █  " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "    " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "    ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "  █ ",
                      " ▀▄     █  ▄▀ ",
                      "   ▀▀▀   ▀▀   "},
                     {"   ▄▄   ▀▀▀▄  ",
                      "▄▀▀  █     ▀▄ ",
                      "█   " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "    " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "    ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "   █",
                      " ▀▄     █  ▄▄▀",
                      "  ▀▄▄▄   ▀▀   "},
                     {"▄▀▀▀▄▄      ▀▄",
                      "      █      █",
                      "    " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "  █ ",
                      "  ▄▄" COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "▀▀  ",
                      " █  " COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "    ",
                      "█      █      ",
                      "▀▄      ▀▀▄▄▄▀"},
                     {"  ▀▀▀▄▄       ",
                      "       █     █",
                      "    " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "  ▄▀",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "▄▀  " COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "    ",
                      "█     █       ",
                      "       ▀▀▄▄▄  "},
                     {"   ▀▀▀▄▄      ",
                      "        █    ▄",
                      "  ▄▄" COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "   █",
                      " █  " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "  █ ",
                      "█   " COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "▀▀  ",
                      "▀    █        ",
                      "      ▀▀▄▄▄   "},
                     {"    ▀▀▀▄▄     ",
                      "         █    ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "   █",
                      "▄▀  " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "  ▄▀",
                      "█   " COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "    █         ",
                      "     ▀▀▄▄▄    "},
                     {"      ▀▀▀▄▄   ",
                      " ▄▀▀▄      █  ",
                      "▄▀  " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "▀   ",
                      "█   " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "   █",
                      "   ▄" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "  ▄▀",
                      "  █      ▀▄▄▀ ",
                      "   ▀▀▄▄▄      "},
                     {"   ▄▄   ▀▀▀▄  ",
                      "▄▀▀  █     ▀▄ ",
                      "█   " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "    " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "    ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "   █",
                      " ▀▄     █  ▄▄▀",
                      "  ▀▄▄▄   ▀▀   "},
                     {"   ▄▄   ▄▄▄   ",
                      " ▄▀  █     ▀▄ ",
                      " █  " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "    " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE "    ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE "  █ ",
                      " ▀▄     █  ▄▀ ",
                      "   ▀▀▀   ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀  █  ▀  ▀▄ ",
                      " ▀▄ " COLOR_PETRIFICATEUR_FIRING "▄█▀▀█▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ " COLOR_PETRIFICATEUR_FIRING,
                      "    █    █    " COLOR_PETRIFICATEUR_BASE,
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█▄▄█▀" COLOR_PETRIFICATEUR_BASE " ▀▄ ",
                      " ▀▄  ▄  █  ▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀  █  █  ▀▄ ",
                      " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█▀▀█▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ " COLOR_PETRIFICATEUR_FIRING,
                      "    █    █    " COLOR_PETRIFICATEUR_BASE,
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█▄▄█▀" COLOR_PETRIFICATEUR_BASE "▀▀▄ ",
                      " ▀▄  █  █  ▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀  █▄▄█  ▀▄ ",
                      " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█  █▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "   █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE "  █   ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█  █▀" COLOR_PETRIFICATEUR_BASE "▀▀▄ ",
                      " ▀▄  █▀▀█  ▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀" COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE " █▄▄█ " COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE "▀▄ ",
                      " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█  █▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "   █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE "  █   ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█  █▀" COLOR_PETRIFICATEUR_BASE "▀▀▄ ",
                      " ▀▄" COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE " █▀▀█ " COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE "▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀" COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE " █▄▄█ " COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE "▀▄ ",
                      " ▀▄▄" COLOR_PETRIFICATEUR_SORON "▄▀▀▀▀▄" COLOR_PETRIFICATEUR_BASE "▄▄▀ ",
                      "   █" COLOR_PETRIFICATEUR_SORON "█ ██ █" COLOR_PETRIFICATEUR_BASE "█   ",
                      " ▄▀▀" COLOR_PETRIFICATEUR_SORON "▀▄▄▄▄▀" COLOR_PETRIFICATEUR_BASE "▀▀▄ ",
                      " ▀▄" COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE " █▀▀█ " COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE "▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀" COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE " █" COLOR_PETRIFICATEUR_SORON "▄▄" COLOR_PETRIFICATEUR_BASE "█ " COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE "▀▄ ",
                      " ▀▄▄" COLOR_BG_SORON COLOR_PETRIFICATEUR_FIRING "▄" COLOR_TOWER_SLOT_BG "▀  ▀" COLOR_BG_SORON "▄" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_BASE "▄▄▀ " COLOR_PETRIFICATEUR_SORON,
                      "   █  ██  █   " COLOR_PETRIFICATEUR_BASE,
                      " ▄▀▀" COLOR_BG_SORON COLOR_PETRIFICATEUR_FIRING "▀" COLOR_TOWER_SLOT_BG "▄  ▄" COLOR_BG_SORON "▀" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_BASE "▀▀▄ ",
                      " ▀▄" COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE " █" COLOR_PETRIFICATEUR_SORON "▀▀" COLOR_PETRIFICATEUR_BASE "█ " COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE "▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {"   ▄▄    ▄▄   ",
                      " ▄▀" COLOR_PETRIFICATEUR_SORON "▄▄" COLOR_PETRIFICATEUR_BASE COLOR_BG_SORON "▄▄▄▄" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_SORON "▄▄" COLOR_PETRIFICATEUR_BASE "▀▄ ",
                      " ▀" COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE COLOR_BG_SORON "▄" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_FIRING "█▀  ▀█" COLOR_PETRIFICATEUR_BASE COLOR_BG_SORON "▄" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE "▀ ",
                      "  " COLOR_PETRIFICATEUR_SORON "█" COLOR_PETRIFICATEUR_BASE "█  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE "  █" COLOR_PETRIFICATEUR_SORON "█  " COLOR_PETRIFICATEUR_BASE,
                      " ▄" COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE COLOR_BG_SORON "▀" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_FIRING "█▄  ▄█" COLOR_PETRIFICATEUR_BASE COLOR_BG_SORON "▀" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE "▄ ",
                      " ▀▄" COLOR_PETRIFICATEUR_SORON "▀▀" COLOR_PETRIFICATEUR_BASE COLOR_BG_SORON "▀▀▀▀" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_SORON "▀▀" COLOR_PETRIFICATEUR_BASE "▄▀ ",
                      "   ▀▀    ▀▀   "},
                     {COLOR_PETRIFICATEUR_SORON "   ▄▄▄▄▄▄▄▄   ",
                      COLOR_PETRIFICATEUR_SORON " ▄▀▄" COLOR_PETRIFICATEUR_BASE " █▄▄█ " COLOR_PETRIFICATEUR_SORON "▄▀▄ ",
                      COLOR_PETRIFICATEUR_SORON " █" COLOR_PETRIFICATEUR_BASE "▄▄" COLOR_PETRIFICATEUR_FIRING "█▀  ▀█" COLOR_PETRIFICATEUR_BASE "▄▄" COLOR_PETRIFICATEUR_SORON "█ ",
                      COLOR_PETRIFICATEUR_SORON " █" COLOR_PETRIFICATEUR_BASE " █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE "  █ " COLOR_PETRIFICATEUR_SORON "█ ",
                      COLOR_PETRIFICATEUR_SORON " █" COLOR_PETRIFICATEUR_BASE "▀▀" COLOR_PETRIFICATEUR_FIRING "█▄  ▄█" COLOR_PETRIFICATEUR_BASE "▀▀" COLOR_PETRIFICATEUR_SORON "█ ",
                      COLOR_PETRIFICATEUR_SORON " ▀▄▀" COLOR_PETRIFICATEUR_BASE " █▀▀█ " COLOR_PETRIFICATEUR_SORON "▀▄▀ ",
                      COLOR_PETRIFICATEUR_SORON "   ▀▀▀▀▀▀▀▀   "}},
                    {

                        {"   ▄▄    ▄▄   ",
                         " ▄▀  █▄▄█  ▀▄ ",
                         " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█  █▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "   █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE_LVL2 "  █   ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█  █▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀▄ ",
                         " ▀▄  █▀▀█  ▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀  █  █  ▀▄ ",
                         " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█▀▀█▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ " COLOR_PETRIFICATEUR_FIRING,
                         "    █    █    " COLOR_PETRIFICATEUR_BASE_LVL2,
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█▄▄█▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀▄ ",
                         " ▀▄  █  █  ▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀  █  ▀  ▀▄ ",
                         " ▀▄ " COLOR_PETRIFICATEUR_FIRING "▄█▀▀█▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ " COLOR_PETRIFICATEUR_FIRING,
                         "    █    █    " COLOR_PETRIFICATEUR_BASE_LVL2,
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█▄▄█▀" COLOR_PETRIFICATEUR_BASE_LVL2 " ▀▄ ",
                         " ▀▄  ▄  █  ▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄   ▄▄▄   ",
                         " ▄▀  █     ▀▄ ",
                         " █  " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "    " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "    ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "  █ ",
                         " ▀▄     █  ▄▀ ",
                         "   ▀▀▀   ▀▀   "},
                        {"   ▄▄   ▀▀▀▄  ",
                         "▄▀▀  █     ▀▄ ",
                         "█   " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "    " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "    ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "   █",
                         " ▀▄     █  ▄▄▀",
                         "  ▀▄▄▄   ▀▀   "},
                        {"▄▀▀▀▄▄      ▀▄",
                         "      █      █",
                         "    " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "  █ ",
                         "  ▄▄" COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀  ",
                         " █  " COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "    ",
                         "█      █      ",
                         "▀▄      ▀▀▄▄▄▀"},
                        {"  ▀▀▀▄▄       ",
                         "       █     █",
                         "    " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "  ▄▀",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "▄▀  " COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "    ",
                         "█     █       ",
                         "       ▀▀▄▄▄  "},
                        {"   ▀▀▀▄▄      ",
                         "        █    ▄",
                         "  ▄▄" COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "   █",
                         " █  " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "  █ ",
                         "█   " COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀  ",
                         "▀    █        ",
                         "      ▀▀▄▄▄   "},
                        {"    ▀▀▀▄▄     ",
                         "         █    ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "   █",
                         "▄▀  " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "  ▄▀",
                         "█   " COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "    █         ",
                         "     ▀▀▄▄▄    "},
                        {"      ▀▀▀▄▄   ",
                         " ▄▀▀▄      █  ",
                         "▄▀  " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▀   ",
                         "█   " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "   █",
                         "   ▄" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "  ▄▀",
                         "  █      ▀▄▄▀ ",
                         "   ▀▀▄▄▄      "},
                        {"   ▄▄   ▀▀▀▄  ",
                         "▄▀▀  █     ▀▄ ",
                         "█   " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "    " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "    ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "   █",
                         " ▀▄     █  ▄▄▀",
                         "  ▀▄▄▄   ▀▀   "},
                        {"   ▄▄   ▄▄▄   ",
                         " ▄▀  █     ▀▄ ",
                         " █  " COLOR_PETRIFICATEUR_FIRING "▄████▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "    " COLOR_PETRIFICATEUR_FIRING "██  ██" COLOR_PETRIFICATEUR_BASE_LVL2 "    ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀████▀" COLOR_PETRIFICATEUR_BASE_LVL2 "  █ ",
                         " ▀▄     █  ▄▀ ",
                         "   ▀▀▀   ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀  █  ▀  ▀▄ ",
                         " ▀▄ " COLOR_PETRIFICATEUR_FIRING "▄█▀▀█▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ " COLOR_PETRIFICATEUR_FIRING,
                         "    █    █    " COLOR_PETRIFICATEUR_BASE_LVL2,
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█▄▄█▀" COLOR_PETRIFICATEUR_BASE_LVL2 " ▀▄ ",
                         " ▀▄  ▄  █  ▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀  █  █  ▀▄ ",
                         " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█▀▀█▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ " COLOR_PETRIFICATEUR_FIRING,
                         "    █    █    " COLOR_PETRIFICATEUR_BASE_LVL2,
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█▄▄█▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀▄ ",
                         " ▀▄  █  █  ▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀  █▄▄█  ▀▄ ",
                         " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█  █▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "   █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE_LVL2 "  █   ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█  █▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀▄ ",
                         " ▀▄  █▀▀█  ▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀" COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE_LVL2 " █▄▄█ " COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▄ ",
                         " ▀▄▄" COLOR_PETRIFICATEUR_FIRING "▄█  █▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "   █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE_LVL2 "  █   ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_FIRING "▀█  █▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀▄ ",
                         " ▀▄" COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE_LVL2 " █▀▀█ " COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀" COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE_LVL2 " █▄▄█ " COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▄ ",
                         " ▀▄▄" COLOR_PETRIFICATEUR_SORON "▄▀▀▀▀▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ ",
                         "   █" COLOR_PETRIFICATEUR_SORON "█ ██ █" COLOR_PETRIFICATEUR_BASE_LVL2 "█   ",
                         " ▄▀▀" COLOR_PETRIFICATEUR_SORON "▀▄▄▄▄▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀▄ ",
                         " ▀▄" COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE_LVL2 " █▀▀█ " COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀" COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE_LVL2 " █" COLOR_PETRIFICATEUR_SORON "▄▄" COLOR_PETRIFICATEUR_BASE_LVL2 "█ " COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▄ ",
                         " ▀▄▄" COLOR_BG_SORON COLOR_PETRIFICATEUR_FIRING "▄" COLOR_TOWER_SLOT_BG "▀  ▀" COLOR_BG_SORON "▄" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄▀ " COLOR_PETRIFICATEUR_SORON,
                         "   █  ██  █   " COLOR_PETRIFICATEUR_BASE_LVL2,
                         " ▄▀▀" COLOR_BG_SORON COLOR_PETRIFICATEUR_FIRING "▀" COLOR_TOWER_SLOT_BG "▄  ▄" COLOR_BG_SORON "▀" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀▄ ",
                         " ▀▄" COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE_LVL2 " █" COLOR_PETRIFICATEUR_SORON "▀▀" COLOR_PETRIFICATEUR_BASE_LVL2 "█ " COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {"   ▄▄    ▄▄   ",
                         " ▄▀" COLOR_PETRIFICATEUR_SORON "▄▄" COLOR_PETRIFICATEUR_BASE_LVL2 COLOR_BG_SORON "▄▄▄▄" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_SORON "▄▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▄ ",
                         " ▀" COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE_LVL2 COLOR_BG_SORON "▄" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_FIRING "█▀  ▀█" COLOR_PETRIFICATEUR_BASE_LVL2 COLOR_BG_SORON "▄" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_SORON "▄" COLOR_PETRIFICATEUR_BASE_LVL2 "▀ ",
                         "  " COLOR_PETRIFICATEUR_SORON "█" COLOR_PETRIFICATEUR_BASE_LVL2 "█  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE_LVL2 "  █" COLOR_PETRIFICATEUR_SORON "█  " COLOR_PETRIFICATEUR_BASE_LVL2,
                         " ▄" COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE_LVL2 COLOR_BG_SORON "▀" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_FIRING "█▄  ▄█" COLOR_PETRIFICATEUR_BASE_LVL2 COLOR_BG_SORON "▀" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_SORON "▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▄ ",
                         " ▀▄" COLOR_PETRIFICATEUR_SORON "▀▀" COLOR_PETRIFICATEUR_BASE_LVL2 COLOR_BG_SORON "▀▀▀▀" COLOR_TOWER_SLOT_BG COLOR_PETRIFICATEUR_SORON "▀▀" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▀ ",
                         "   ▀▀    ▀▀   "},
                        {COLOR_PETRIFICATEUR_SORON "   ▄▄▄▄▄▄▄▄   ",
                         COLOR_PETRIFICATEUR_SORON " ▄▀▄" COLOR_PETRIFICATEUR_BASE_LVL2 " █▄▄█ " COLOR_PETRIFICATEUR_SORON "▄▀▄ ",
                         COLOR_PETRIFICATEUR_SORON " █" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄" COLOR_PETRIFICATEUR_FIRING "█▀  ▀█" COLOR_PETRIFICATEUR_BASE_LVL2 "▄▄" COLOR_PETRIFICATEUR_SORON "█ ",
                         COLOR_PETRIFICATEUR_SORON " █" COLOR_PETRIFICATEUR_BASE_LVL2 " █  " COLOR_PETRIFICATEUR_SORON "██" COLOR_PETRIFICATEUR_BASE_LVL2 "  █ " COLOR_PETRIFICATEUR_SORON "█ ",
                         COLOR_PETRIFICATEUR_SORON " █" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀" COLOR_PETRIFICATEUR_FIRING "█▄  ▄█" COLOR_PETRIFICATEUR_BASE_LVL2 "▀▀" COLOR_PETRIFICATEUR_SORON "█ ",
                         COLOR_PETRIFICATEUR_SORON " ▀▄▀" COLOR_PETRIFICATEUR_BASE_LVL2 " █▀▀█ " COLOR_PETRIFICATEUR_SORON "▀▄▀ ",
                         COLOR_PETRIFICATEUR_SORON "   ▀▀▀▀▀▀▀▀   "}}};
                int petrificateur_shooting; // Même principe que pour la gatling, calcul de quel sprites par rapport à l'horloge interne mais cette fois pour 20 sprites
                petrificateur_shooting = floor(cell.turret.compteur * (20 / cell.turret.reload_delay[cell.turret.lvl]));
                if (petrificateur_shooting > 19.99)
                {
                    petrificateur_shooting = 19;
                }

                if (cell.turret.in_range == false)
                {
                    petrificateur_shooting = floor(petrificateur_shooting / 4) + 5;
                }

                for (int i = 0; i < 7; i++)
                {
                    move_to(x_current_turret, y_current_turret + i);
                    printf(sprite[cell.turret.lvl][petrificateur_shooting][i]);
                }
            }
            else if (cell.turret.type == Banque) // Sprites de la banque (avec animation lorsque l'argent apparait)
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
                      "    █ +2 █    ",
                      "    █▄▄▄▄█    ",
                      "              ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "   ▄▄▄▄▄▄▄▄   ",
                      "   █      █   ",
                      "   █  +2  █   ",
                      "   █      █   ",
                      "   ▀▀▀▀▀▀▀▀   ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "  █▀▀▀▀▀▀▀▀█  ",
                      "  █        █  ",
                      "  █   +2   █  ",
                      "  █        █  ",
                      "  █▄▄▄▄▄▄▄▄█  ",
                      "              "},
                     {COLOR_BANQUE_MONEY " ▄▄▄▄▄▄▄▄▄▄▄▄ ",
                      " █          █ ",
                      " █     ▛▀▜  █ ",
                      " █ ▟▙   ▄▀  █ ",
                      " █ ▜▛  ▟▄▄  █ ",
                      " █          █ ",
                      " ▀▀▀▀▀▀▀▀▀▀▀▀ "},
                     {COLOR_BANQUE_MONEY "█▀▀▀▀▀▀▀▀▀▀▀▀█",
                      "█            █",
                      "█      ▛▀▜   █",
                      "█  ▟▙   ▄▀   █",
                      "█  ▜▛  ▟▄▄   █",
                      "█            █",
                      "█▄▄▄▄▄▄▄▄▄▄▄▄█"},
                     {COLOR_BANQUE_GENERATION "█▀▀▀▀▀▀▀▀▀▀▀▀█",
                      "█            █",
                      "█      ▛▀▜   █",
                      "█  ▟▙   ▄▀   █",
                      "█  ▜▛  ▟▄▄   █",
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
                      "    █ +6 █    ",
                      "    █▄▄▄▄█    ",
                      "              ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "   ▄▄▄▄▄▄▄▄   ",
                      "   █      █   ",
                      "   █  +6  █   ",
                      "   █      █   ",
                      "   ▀▀▀▀▀▀▀▀   ",
                      "              "},
                     {COLOR_BANQUE_MONEY "              ",
                      "  █▀▀▀▀▀▀▀▀█  ",
                      "  █        █  ",
                      "  █   +6   █  ",
                      "  █        █  ",
                      "  █▄▄▄▄▄▄▄▄█  ",
                      "              "},
                     {COLOR_BANQUE_MONEY " ▄▄▄▄▄▄▄▄▄▄▄▄ ",
                      " █          █ ",
                      " █     ▛▀▀  █ ",
                      " █ ▟▙  ▛▀▜  █ ",
                      " █ ▜▛  ▙▄▟  █ ",
                      " █          █ ",
                      " ▀▀▀▀▀▀▀▀▀▀▀▀ "},
                     {COLOR_BANQUE_MONEY "█▀▀▀▀▀▀▀▀▀▀▀▀█",
                      "█            █",
                      "█      ▛▀▀   █",
                      "█  ▟▙  ▛▀▜   █",
                      "█  ▜▛  ▙▄▟   █",
                      "█            █",
                      "█▄▄▄▄▄▄▄▄▄▄▄▄█"},
                     {COLOR_BANQUE_GENERATION "█▀▀▀▀▀▀▀▀▀▀▀▀█",
                      "█            █",
                      "█      ▛▀▀   █",
                      "█  ▟▙  ▛▀▜   █",
                      "█  ▜▛  ▙▄▟   █",
                      "█            █",
                      "█▄▄▄▄▄▄▄▄▄▄▄▄█"}}};
                int banque_generating; // Calcul de quand l'animation doit être jouée (floor() = partie entière)
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
            }
            printf(RESET);
        }
    }
    if (cell.type == CHEMIN)
    {
        // Détermination du type de chemin (pour bien dessiner leurs bordures)
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

            for (int x = 0; x < CELL_WIDTH + 2; x++) // Dessin des chemins selon leurs status
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
void update_grid(){
    // for (int x = 0; x < grid.width; x++)
    // {
    //     for (int y = 0; y < grid.height; y++)
    //     {
    //         grid.cells[x][y].has_effect = false;
    //         grid.cells[x][y].effect = 0;
    //         grid.cells[x][y].effect_counter = -1;
    //     }
    // }
}
void drawFullGrid(Grid grid) // Dessine la grille en entière
{

    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++)
        {
            drawCell(grid.cells[x][y], grid);
        }
    }
}
void clearPath(Grid grid) // Dessine uniquement les chemins
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

void clearUsedPath(Grid grid, EnemyPool ep) // Redessine les endroits aves des ennemis
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
                        (ep.enemies[i].previous_cell.x == x && ep.enemies[i].previous_cell.y == y)        //
                        || (ep.enemies[i].previous_cell.x + 1 == x && ep.enemies[i].previous_cell.y == y) //
                        || (ep.enemies[i].previous_cell.x == x && ep.enemies[i].previous_cell.y + 1 == y))
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