# ATRDS
Antique Terminal Route Defense Simulator

- [ATRDS](#atrds)
  - [Resources utiles](#resources-utiles)
  - [Démarage](#démarage)
    - [Récupérer le code](#récupérer-le-code)
      - [Depuis github](#depuis-github)
      - [Depuis caséine](#depuis-caséine)
    - [Compilation](#compilation)
      - [Automatique](#automatique)
      - [Manuelle](#manuelle)
    - [Lancer le jeu](#lancer-le-jeu)
      - [Paramètres de démarrage](#paramètres-de-démarrage)
  - [Documentation](#documentation)
    - [Aide a la correction:](#aide-a-la-correction)
    - [Liste des fonctionnalités](#liste-des-fonctionnalités)
    - [Architecture du projet](#architecture-du-projet)
      - [Division du code](#division-du-code)
      - [Éléments nouveaux](#éléments-nouveaux)
      - [Algorithmes originaux](#algorithmes-originaux)
      - [Allocations dynamiques](#allocations-dynamiques)
        - [Sécurité](#sécurité)
        - [Grille](#grille)
      - [Affichage](#affichage)
      - [Le problème des missiles](#le-problème-des-missiles)
    - [Simulation](#simulation)

## Resources utiles
- Documentation C: [cppreference](https://en.cppreference.com/w/c)
- [UNICODE, caractères utiles](https://en.wikipedia.org/wiki/List_of_Unicode_characters#Block_Elements)
- Codes ANSI
  - [gist/github](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
  - [Wikipedia](https://en.wikipedia.org/wiki/ANSI_escape_code)

## Démarage

### Récupérer le code

#### Depuis github
Le plus simple et l'option recommandée pour avoir l’**intégralité du code**, c'est d'utiliser l'outil **git** disponible via [apt](https://git-scm.com/book/fr/v2/D%C3%A9marrage-rapide-Installation-de-Git) et [pacman](https://wiki.archlinux.org/title/Git), [git-scm](https://git-scm.com/downloads/win) sur windows.
```bash
git clone https://github.com/rafalou38/ATRDS.git
```

Ou de télécharger ce zip manuellement: https://github.com/rafalou38/ATRDS/archive/refs/heads/master.zip

#### Depuis caséine
Le code est également disponible dans les téléchargements caséine, mais vous n'aurez pas toute l'architecture du projet ni le script de compilation.
> ⚠️ Les scripts sont combinés sur caséine, pour avoir des fichiers plus simples, passez par git.
> 
> Il y manque également toute la partie simulation ainsi que les scripts.

### Compilation

#### Automatique
Si vous avez utilisé git, il suffit d'éxecuter le fichier `run.sh`
```bash
./build.sh
```

#### Manuelle
Si vous ne l'avez pas, placez les fichiers code dans le dossier courant puis compilez les de la sorte:
```bash
gcc -o atrds.out -std=c99 src/main.c src/grid.c src/shared.c src/enemies.c src/tower.c -lm
```

### Lancer le jeu

```bash
./atrds.out
```

Si sur la version git les scripts `run.sh` et `build.sh` sont disponibles.

`run.sh` recompile automatiquement le jeu a chaque execution.

#### Paramètres de démarrage

| paramètre  | description                                    |
| ---------- | ---------------------------------------------- |
| -h         | affiche le tutoriel                            |
| --play     | lance le jeu directement                       |
| --sim-wave | Bonus: voir [l’annexe simulation](#simulation) |

Si aucun paramètre n'est défini, le jeu se lancera sur le menu standard.


## Documentation

### Aide a la correction:
Nous sommes conscient que c'est un gros projet, pour te faciliter la tache voila une liste d'elements que tu peux facilement évaluer.
Si tu est dans vscode, tu dois pouvoir ctrl+click sur les liens pour aller directement au fichier et a la ligne correspondante.
- **Critère 1**: 
    - Disponible en version condensée sur caséine 
    - Sur github, plusieurs fichiers pour améliorer la lisibilité: `enemies.c`  `enemies.h`  `grid.c`  `grid.h`  `main.c`  `shared.c`  `shared.h`  `tower.c`  `tower.h`
- **Critère 2**: voir [Compilation](#compilation) et [Lancer le jeu](#lancer-le-jeu)
- **Critère 3**: Un tutoriel est intégré au jeu.
- **Critère 4**: [liste des fonctionnalités](#liste-des-fonctionnalités)
- **Critère 5**: A vous de tester :wink:.
- **Critère 6**: On a des variables et des fonctions dans tous les fichiers, a toi de naviguer :boat:.
- **Critère 7**: Les entrées sorties sont gérés via [main.c / gestion des entrees clavier (ligne 300)](./src/main.c#l300)
- **Critère 8**: Allocation de la grille: voir [Architecture du projet/Allocations dynamiques/grille](#grille)
- **Critère 9**: Gestion de l'affichage voir [Architecture du projet/affichage](#affichage)
- **Critère 10**: Verification du retour des mallocs: voir [Architecture du projet/Allocations dynamiques/sécurité](#sécurité)

### Liste des fonctionnalités

- Vérifie que la taille du terminal convient
- Menu de jeu `shared.c/anim_debut`
    - Introduit par une animation (peut ne pas fonctionner sur tous les terminaux)
    - Bouton "Jouer"
        - Lance la partie
    - Bouton "tutoriel"
        - Affiche un petit tutoriel sur comment fonctionne le jeu
- Jeu
    - Un chemin aléatoire est généré automatiquement `grid.c/genBasicPath`
    - Les ennemis arrivent progressivement selon un système de vagues précis
    - Le jeu se termine lorsque la vie de l'utilisateur atteint 0
        - Il pert ses PV quand un ennemi atteint la fin du chemin
    - Un curseur, déplacé avec les fleches du clavier, permet de sélectionner une case.
        - `espace` affiche son menu
            - Sélection de l'option avec les fleches du clavier
            - `enter` valider l'option
            - `espace` fermer le menu
            - Case vide: liste des tourelles (magasin)
                - Les tourelles sont achetées avec la monnaie du jeu
            - Case occupée: menu d'amélioration
                - Vendre: revends la tourelle pour une certaine valeur
                - Amélioration: si disponible, augmente les stats de la tourelle
    - Les tourelles placées attaquent automatiquement les ennemis en fonction de leurs caractéristiques
        - Les ennemis morts apportent de la monnaie
        - Pour la liste des tourelles, voir le tuto

La meilleure façon de tester tout ça, c'est de jouer :joystick: !

### Architecture du projet

#### Division du code
Le projet est divisé entre des fichiers `.h` et des fichiers `.c`.
Les fichiers `.h` contiennent les constantes, les noms de fonctions, types customisés, structs, enums, etc.
Les fichiers `.c` contiennent le code du jeu et sont inclus au moment de la compilation.

**Par exemple:**
> **grid.h** contient la définition de la fonction avec valeur de retour et paramètres, il est importé par les `#include`, et expose ses fonctions pour qu'elles puissent être utilisées par d'autres fichiers.
```c
void fillBG(int xmin, int ymin, int xmax, int ymax);
```
> **grid.c** le code de la fonction est automatiquement inclut a la compilation par la présence de:
> `gcc -o atrds.out -std=c99 src/main.c [src/grid.c] src/shared.c src/enemies.c src/tower.c -lm`
> il n'est pas inclus par les `#include`, car c a uniquement besoin de la "signature" de la fonction pour verifier la validité programme.
```c
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
```

**Liste de tous les struct, enum et fonctions:**
- enemies.c
    - AllocEnemyPool 
    - freeEnemyPool
    - defEnemy: *Renvoie le struct ennemi initialisé*
    - addEnemy: *ajoute un ennemi a la liste d'ennemis*
    - defragEnemyPool: *voir [tableau comme liste](#algorithmes-originaux)*
    - drawEnemies
    - updateEnemies
    - switchToWave
    - getWaveByIndex: *renvoie le struct wavePattern d'une vague initialisé*
    - updateWaveSystem
    - testWaveSystem: *voir [Simulation](#simulation)*
- enemies.h
    - enum EnemyType
    - enum EnemyState: *Vivant mort, arrivé*
    - struct Enemy
    - struct WavePattern
    - struct WaveSystem
- grid.c
    - allocateGridCells
    - freeGrid
    - genBasicPath
    - fillBG
    - drawCell
    - drawFullGrid
    - clearPath
    - clearUsedPath
- shared.h
    - enum TurretType
    - enum EffectType: *effets de tourelle*
    - struct Turret
    - struct EnemyPool
    - enum CellType
    - struct Cell
    - struct Grid
    - struct GameStats
    - struct Label
    - struct Labels
    - #define HISTORY_SIZE
    - #define MIN_TERMINAL_WIDTH
    - #define MIN_TERMINAL_HEIGHT
    - #define MAX_TERMINAL_WIDTH
    - #define MAX_TERMINAL_HEIGHT
    - #define TARGET_FPS
    - #define WAVE_DELAY
    - #define CELL_WIDTH
    - #define CELL_HEIGHT
    - #define PI
    - TOUTES LES COULEURS
- shared.c
    - get_key_press: *renvoie la valeur de la touche clavier appuyée.*
    - move_to: *déplacement du curseur.*
    - clear_screen
    - clear_line
    - hide_cursor
    - show_cursor
    - printCritical: *affiche un message d'erreur.*
    - get_terminal_size
    - checkTerminalSize: *bloque tant que la taille du terminal n'est pas correcte*
    - drawRange: *dessine les cercles de portée des tourelles.*
    - anim_debut
- tower.h
    - struct Bullet: Voir [le problème des missiles](#le-problème-des-missiles)
    - struct BulletPool
    - define BULLETS_ON
- tower.c
    - showTowerSelection *print le menu de selection de tourelle*
    - getTurretStruct *renvoie le struct initialisé de la tourelle*
    - getTurretPrice *renvoie le prix de la tourelle*
    - updateTowers
    - drawBullets
    - updateBullets
    - updateTowers

#### Éléments nouveaux
**Enum**, pareil que les types somme en ocaml.
```c
enum TurretType
{
    Sniper,
    Inferno,
    Mortier,
    Freezer,
    Petrificateur,
    Banque,
    Gatling,
};
enum TurretType type = Mortier;
```
**Struct** vu en cours
**typedef struct** (version raccourcie)
```c
struct Enemy // version standard 
{
    enum EnemyType type;
    float hp;
    float maxHP;
    float speed;
    int damage;
    int money;
};
// on définit le type et le struct en même temps
typedef struct EnemyPool
{
    int length; // Taille du tableau
    int count;  // Nombre d’ennemis dans le tableau
    struct Enemy *enemies;
} EnemyPool;

// Typedef: on mets directement le nom
EnemyPool enemyPool;
// Sinon, il faut rajouter "struct"
struct Enemy enemy;
```

**macro**
Une macro (par exemple MAX et MIN) permet d'écrire une fonction sans pour autant avoir à spécifier les types.
Cependant, il faut faire attention a ne pas oublier les parentaises car une macro ne fait que remplacer la macro par
ce par quoi on l'a définie (c'est a dire comme les défine). Donc en oubliant les parentaises, on risque de changer completement la valeur retenue par le programme.
**ternaire**
Le ternaire est le ? des macro.
Le ? permet de savoir si la condition précédente est vraie ou non puis les : permetent de determiner quoi faire en fonction de la véracité de la condition.
Si la condition est vrai, la macro renvera ce qui se trouve entre le ? et les :.
Sinon, la macro renvera ce qui se trouve apres les :. 

#### Algorithmes originaux
**Génération du chemin**: genBasicPath [./src/grid.c](./src/grid.c#L57)
Cet algorithme crée un chemin continu dans une grille en suivant des règles pour éviter les collisions et en utilisant un historique pour revenir en arrière en cas de blocage.

1. On choisit une position aléatoire sur le côté gauche de la grille (x = 0).
2. On place deux premières cases de chemin, créant le point de départ.
3. On initialise un historique des positions visitées, permettant de revenir en arrière si aucun déplacement n'est possible.

4. Tant que le chemin n'a pas atteint le côté droit de la grille (x = grid->width - 1), on vérifie les directions dans lesquelles il peut se déplacer (haut, bas, droite, gauche).
Chaque direction doit respecter certaines conditions pour éviter que le chemin ne colle à d'autres cases de chemin ou n’entre en collision avec les bords.
Si aucune direction n'est possible, on recule en revenant à la dernière position valable grace a l'historique.
5. On choisit aléatoirement parmi les directions disponibles avec une plus haute probabilité vers la gauche et on y avance.

**Tableau comme liste**
Les listes ne sont pas disponibles, il faut donc se débrouiller avec les tableaux. Pour ça, on a opté pour la solution suivante dans le cas de:
- Liste des ennemis
- Liste des labels
- Liste des missiles ([problème des missiles](#le-probleme-des-missiles))

On définit un struct qui va représenter la liste et qui contient le tableau avec:
- tableau (allocation dynamique)
- taille du tableau
- nombre d'elements

Le nombre d'elements n'est pas égal a la taille du tableau.
Taille du tableau correspond a la taille de l'allocation et donc au nombre maximum d'elements.
On a donc toujours l'intégralité des elements d'allouées mais en considérant "nombre d'elements" on considère que les suivants n'existent pas, et en effet, ils ne sont pas initialisées.

Pour **ajouter** un nouvel element:
- on l'assigne a la case correspondant au nombre d'elements
- on incrémente le nombre d'elements
```c
lst.elements[lst.count] = element;
lst.count++;
```

Pour **supprimer** un element:
cette opération est beaucoup plus difficile a réaliser a cause du tableau.
On doit marquer les elements a supprimer puis parcourir le tableau en décalant tous les non marquées vers la gauche pour remplir le cases vides.
On a choisi le nom de défragmentation pour cet algorithme en reference a https://fr.wikipedia.org/wiki/D%C3%A9fragmentation_(informatique)
```c
void defragEnemyPool(EnemyPool *ep)
{
    int right = 0;
    int left = -1;

    while (right < ep->count)
    {
        if (ep->enemies[right].state == ENEMY_STATE_ALIVE)
        {
            if (left != -1)
            {
                assert(left >= 0);
                assert(left < ep->length);

                ep->enemies[left] = ep->enemies[right];
                left++;
            }
        }
        else if (left == -1)
        {
            left = right;
        }

        right++;
    }
    ep->count -= right - left;
}
```


#### Allocations dynamiques
De nombreuses allocations dynamiques sont utilisées dans le jeu.
- Tableau des ennemis: `AllocEnemyPool` [./src/enemies.c](./src/enemies.c#L3)
- Historique du chemin: [./src/grid.c](./src/grid.c#L88)
- Labels: [./src/main.c](./src/main.c#L332)
- Grille: [./src/grid.c](./src/grid.c#L12) et [./src/grid.c](./src/grid.c#L21)

##### Sécurité
A chaque malloc que nous avons utilisé, nous avons pensé à:
-verifier que le malloc ne renvoie pas NULL
-free les malocs plus utilisé.
On free
- Le tableau des ennemis lors de la fin du jeu [./src/main.c](./src/main.c#L545)
- L'historique du chemin après la génération du chemin [./src/grid.c](./src/grid.c#L179)
- Les labels lors de la fin du jeu [./src/main.c](./src/main.c#L546)
- La grid lors de la fin du jeu [./src/main.c](./src/main.c#L544) 
##### Grille
La grille est un tableau dynamique trouvée dans la structure grid.
Ce tableau en deux dimentions: la première gére la ligne de la case
tandit que la seconde gère sa collonne.
Le tableau de pointeur grid.cells gère cette grille et permet de savoir le type(c'est a dire savoir si c'est un terrain ou un chemin), ainsi que ce que contient la case.
#### Affichage
L'affiche fonctionne de manière différente que la grille:
La fonction drawFullGrid [./src/grid.c](./src/grid.c#L1322) permet d'afficher toute la grille en parcours grid.cells et appelle la fonction drawCell[./src/grid.c](./src/grid.c#L195) pour chaque cellule
La fonction drawCell distingue le type de case a afficher puis détermine si elle doit afficher une tourelle ou non, si oui, laquelle elle doit afficher.
La fonction drawEnemies affiche les énnemis dans le chemin et determine quel ennemi afficher et donc quel sprite afficher et a quel endroit  
#### Le problème des missiles
Lorsqu'on active les bullet, les bullet réecrive sur les cells ce qui fait que tant qu'on ne redraw pas la grille, les balles resteront sur la grille mêmesi elles ont changer de position.
Cependant, si on redraw toute la grille, le terminal commence a laguer et a avoir du mal a afficher correctement toute la grille.
Vous pouvez donc choisir si on active ou non les bullet, mais le jeu aura des problèmes d'affichage.
### Simulation
La similation est un bonus que nous avous rajouté qui permet de creer une simulation pour voir comment le jeu crée les vagues d'ennemis.