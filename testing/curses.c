#include <curses.h>
#include <stdlib.h>

int main(void)
{
    initscr(); // Initialise la structure WINDOW et autres paramètres
    while (1)
    {
        erase();
        for (int x = 0; x < COLS; x++)
        {
            for (int y = 0; y < LINES; y++)
            {
                move(y, x); // Déplace le curseur tout en bas à droite de l'écran
                if (rand() % 3 == 0)
                {
                    addch('o'); // Écrit le caractère . au nouvel emplacement
                }
                else
                {
                    addch('.'); // Écrit le caractère . au nouvel emplacement
                }
            }
        }
        refresh();
        // getch();  // On attend que l'utilisateur appui sur une touche pour quitter
    }

    endwin(); // Restaure les paramètres par défaut du terminal

    return 0;
}