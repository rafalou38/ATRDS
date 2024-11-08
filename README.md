# ATARDES
Antique Terminal : Absolute Routing Defense Epic Simulator

> 🛑 Ne t'embette pas a lire cette documentation sur caséine, elle est disponible sous une forme beaucoup plus agréable ici: https://github.com/rafalou38/atrds

- [ATARDES](#atardes)
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
      - [Comment jouer](#comment-jouer)
  - [Documentation](#documentation)
    - [Aide a la correction:](#aide-a-la-correction)
    - [Liste des fonctionnalités](#liste-des-fonctionnalités)
    - [Architecture du projet](#architecture-du-projet)
      - [Division du code](#division-du-code)
      - [Éléments nouveaux](#éléments-nouveaux)
      - [Algorithmes originaux](#algorithmes-originaux)
        - [**Génération du chemin**:](#génération-du-chemin)
        - [**Tableau comme liste**](#tableau-comme-liste)
      - [Allocations dynamiques](#allocations-dynamiques)
        - [Sécurité](#sécurité)
        - [Grille](#grille)
      - [Affichage](#affichage)
      - [Le problème des missiles](#le-problème-des-missiles)
    - [Système de vagues](#système-de-vagues)
      - [Simulation](#simulation)

## Resources utiles
- Documentation C: [cppreference](https://en.cppreference.com/w/c)
- [UNICODE, caractères utiles](https://en.wikipedia.org/wiki/List_of_Unicode_characters#Block_Elements)
- Codes ANSI
  - [gist/github](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
  - [Wikipedia](https://en.wikipedia.org/wiki/ANSI_escape_code)

## Démarage

Si tu rencontre un quelconque souci avec le projet, tu peux contacter Rafael (@rafalou38) Rémi (@diotduvin) ou Philémon (@nomelip) on sera contents de t'aider.

### Récupérer le code

Si tu n'as pas accès a un environment de code fonctionnel pour tester le jeu tu peux te connecter a ce serveur ou tout prêt pour compiler.
Avec la commande suivante qui doit etre disponible sur windows linux et mac.
```
ssh projet-info@rf38.ddns.net
``` 
pour le mot de passe, contactez un de nous.
Mot de passe: 

Pour lire le code tu peux aussi te rendre sur cette url tout simplement: https://github.com/rafalou38/atrds

#### Depuis github

Le plus simple et l'option recommandée pour avoir l’**intégralité du code en local**, c'est d'utiliser l'outil **git** disponible via [apt](https://git-scm.com/book/fr/v2/D%C3%A9marrage-rapide-Installation-de-Git) et [pacman](https://wiki.archlinux.org/title/Git), [git-scm](https://git-scm.com/downloads/win) sur windows.
```bash
git clone https://github.com/rafalou38/ATRDS.git
```
> C'est ce qui a été fait sur le serveur.

Ou de télécharger ce zip manuellement: https://github.com/rafalou38/ATRDS/archive/refs/heads/master.zip


#### Depuis caséine
Le code est également disponible dans les téléchargements caséine, mais vous n'aurez pas toute l'architecture du projet ni le script de compilation.
> ⚠️ Les scripts sont combinés sur caséine, pour avoir des fichiers plus simples. Il est conseillé de passer par git.
> 
> Il y manque également toute la partie simulation ainsi que les scripts.

### Compilation
N’essayez pas de le compiler sur caséine, vous pourrez le compiler mais l’exécution dans le navigateur semble ne pas fonctionner.

Privilégiez une execution locale ou l'usage du serveur public fourni.

#### Automatique
Si vous avez utilisé git, il suffit d'éxecuter le fichier `run.sh`
```bash
./build.sh
```

#### Manuelle
Si vous ne l'avez pas fait, placez tous les fichiers code dans le dossier courant puis compilez les de la sorte:
```bash
gcc -o atrds.out -std=c99 main.c grid.c shared.c enemies.c tower.c -lm
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
| --play     | lance le jeu directement                       |
| --sim-wave | Bonus: voir [l’annexe simulation](#simulation) |

Si aucun paramètre n'est défini, le jeu se lancera sur le menu standard.

Exemple:
```bash
./run.sh --play
# qui équivaut à
./build.sh
./atrds.out --play 
```

#### Comment jouer
Le jeu vous présente tout d'abord un menu avec deux options: tutoriel et jouer.
Commencez par lire le tutoriel qui vous présentera le lore du jeu ainsi que les types de tourelles.

Vous êtes ensuite lancées dans le jeu avec 20HP et 30€:
- déplacez le curseur sur une case avec les flèches du clavier puis pressez la touche `espace` pour ouvrir le magasin
- Sélectionnez la tourelle de votre choix avec les fleches `haut`/`bas` puis `entrer` pour valider
- Vous pouvez re-sélectionner une tourelle pour l'améliorer ou la vendre.
- Pour quitter le jeu: touche `q`
- Vous découvrirez les ennemis et leurs particularités en jouant :wink:  

## Documentation

### Aide a la correction:
Nous sommes conscient que c'est un gros projet, pour te faciliter la tache voila une liste d'elements que tu peux facilement évaluer.
Si tu est dans vscode, tu dois pouvoir ctrl+click sur les liens pour aller directement au fichier et a la ligne correspondante.
- **Critère 1**: 
    - Disponible en version condensée sur caséine ( on aurait pu tout mettre dans un fichier de 4.000 Lignes mais c'est moin pratique pour se retrouver).
    - Sur github, plusieurs fichiers pour améliorer la lisibilité: `enemies.c`  `enemies.h`  `grid.c`  `grid.h`  `main.c`  `shared.c`  `shared.h`  `tower.c`  `tower.h`
- **Critère 2**: voir [Compilation](#compilation) et [Lancer le jeu](#lancer-le-jeu)
- **Critère 3**: Un tutoriel est intégré au jeu. [Comment jouer](#comment-jouer)
- **Critère 4**: [liste des fonctionnalités](#liste-des-fonctionnalités)
- **Critère 5**: A vous de tester :wink:.
- **Critère 6**: On a des variables et des fonctions dans tous les fichiers, a toi de naviguer :boat:.
- **Critère 7**: Les entrées sorties sont gérés via [main.c / gestion des entrees clavier (ligne 507)](./src/main.c#l507)
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
    - Les ennemis arrivent progressivement selon un système de vagues précis [Système de vagues](#système-de-vagues)
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
Toutes les fonctions qui n'ont pas un nom explicite sont expliquée lors de leur définition et chaque fonctions possède differents commentaires expliquant comment elle fonctionne et ce que fait les lignes de code compliquées directement dans le code.
- enemies.c
    - AllocEnemyPool 
    - freeEnemyPool
    - defEnemy: *Renvoie le struct ennemi initialisé*
    - addEnemy: *ajoute un ennemi a la liste d'ennemis*
    - defragEnemyPool: *voir [tableau comme liste](#algorithmes-originaux)*
    - drawEnemies
    - updateEnemies
    - switchToWave *voir [Système de vagues](#système-de-vagues)*
    - getWaveByIndex: *renvoie le struct wavePattern d'une vague initialisé*
    - updateWaveSystem
    - testWaveSystem: *voir [Simulation](#simulation)*
- enemies.h
    - enum EnemyType
    - enum EnemyState: *Vivant, mort, arrivé*
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


**Opérateur ternaire**
L'opérateur termnaire "?" permet de faire des IF de la même façon qu'en ocaml.

Si la condition précédant le "?" est vraie la valeur qui suit le "?" sera retournée sinon celle qui suit les ":" sera retournée a la place.

```ocaml
let x = if n>4 then 5 else -1;;
```
équivaut a 
```ocaml
int x =  n>4 ? 5 : -1;
```
**macro**
Une macro (par exemple MAX et MIN) permet d'écrire une fonction sans pour autant avoir à spécifier les types. Elle sera directement insérée dans le code au moment de la compilation.
Cependant, il faut faire attention a ne pas oublier les parenthèses car étant donné qu'elle est simplement insérée dans le code, l’ordre des opérations peut ne pas ètre celui voulu.

Exemple:
```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int x = 4;
x = MAX(x/2.0f, 3.0f);
// Sera remplacé a la compilation par
x = ((x/2.0f) > (3.0f) ? (x/2.0f) : (3.0f));
```

#### Algorithmes originaux
##### **Génération du chemin**: 
genBasicPath [./src/grid.c](./src/grid.c#L57)
Cet algorithme crée un chemin continu dans une grille en suivant des règles pour éviter les collisions et en utilisant un historique pour revenir en arrière en cas de blocage.

1. On choisit une position aléatoire sur le côté gauche de la grille (x = 0).
2. On place deux premières cases de chemin, créant le point de départ.
3. On initialise un historique des positions visitées, permettant de revenir en arrière si aucun déplacement n'est possible.

4. Tant que le chemin n'a pas atteint le côté droit de la grille (x = grid->width - 1), on vérifie les directions dans lesquelles il peut se déplacer (haut, bas, droite, gauche).
Chaque direction doit respecter certaines conditions pour éviter que le chemin ne colle à d'autres cases de chemin ou n’entre en collision avec les bords.
Si aucune direction n'est possible, on recule en revenant à la dernière position valable grace a l'historique.
5. On choisit aléatoirement parmi les directions disponibles avec une plus haute probabilité vers la gauche et on y avance.

##### **Tableau comme liste**
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
- Tableau de la grille: [./src/grid.c](./src/grid.c#L4)
- Tableau des labels:   [./src/main.c](./src/main.c#L331)
- Historique du chemin: [./src/grid.c](./src/grid.c#L88)

##### Sécurité
Le programme libère automatiquement les tableaux à la fin du programme (Soit si on perd, soit si on quitte avec 'q')
Dans main.c ligne 535 : [./src/main.c](./src/main.c#L535)
```c
/*
    ################
    FIN DE LA PARTIE
    ################
    */

    printf("\n");
    clear_screen();
    move_to(0, 0);
    freeGrid(grid);           // Libère les allocations dynamiques liées à la grille
    freeEnemyPool(enemyPool); // Libère les allocations dynamiques liées aux ennemis
    freeLabels(labels);       // Libère les allocations dynamiques liées aux labels

```
Pour ce qui est de l'historique du chemin, le tableau est libéré à la fin de la fonction qui génère le chemin.
Dans grid.c ligne 177 : [./src/grid.c](./src/main.c#L177)
```c
for (size_t i = 0; i < HISTORY_SIZE; i++) // Libération de l'historique à la fin de la création de chemin
    {
        free(historique[i]);
    }
    free(historique);
```
##### Grille
La grille est un tableau a deux dimensions ([allocation](./src/grid.c#L4)) de type `struct Cell **`.
Sa taille est définie par la taille du terminal et des cases.
Elle est consérvée dans le struct `Grid`:
```c
typedef struct Grid
{
    int width; // Largeur en cases
    int height; // hauteur en cases
    int start_x; // position de la première case du chemin
    int start_y;
    int end_x; // position de la dernière case du chemin
    int end_y;
    struct Cell **cells;
} Grid;
```

#### Affichage
Pour l'afficage de la grille, plusieurs systèmes entrent en jeu:
- drawCell affiche une case inividuelle; elle distingue le type de la case 
    - Si c'est une tourelle on affiche son prite avec les animations
    - si c'est une case de chemin, on efface son centre ét on affiche la bordure en fonction des cases alentour.
    - Si c'est un case sans tourelle et sans chemin, on affiche un carré 
    - On affihce aussi un bordure de couleur si la case est séléctionée 
- drawFullGrid appele drawCell sur tooutes les cases
- drawUsedPath affiche drawCell seulement sur les cases qui ont besoin d'etre réaffichées
La fonction drawEnemies parcours les enemis et les afiches un a un avec leurs animations.


#### Le problème des missiles
Un des plus gros probèmes auquels nous avons été confrontès avec le términal est la latence.
En effet, les écritures sont très lentes et il n'est donc pass possible de réafficher 'intégralité des caractères du terminal a chaque frame (~60x par seconde). Le clignotement rendrait le jeu injuable.

On doit donc faire des compromis et réafficher seulement des zones réduites du terminal. C'est ce qui justifie la présence d’artefacts (charactères qui sont réstées affichès et qui ne devraient pas exister).

Nous avions implémenté un système de missiles (charactères tirées par les tourelles qui volent en direction de l’ennemi). Le problème c'est qu'on affiche donc des caractères sur tout l'écran et qu'un fois qu'un caractère est affiché, on ne peut pas réstoaurer celui qui se trouvait a sa place. On est donc forcé de laisser les traces des missiles ou de nettoyer tout l'écran régulièrement.


On a donc décidé de désactiver les missiles.Vous pouvez cependant tout de mème les avoir en définissant `#define BULLETS_ON true` en [tower.h:6](./src/tower.h#L6) 

:warning: ce n'est pas prévu et c'est donc normal que ça ne fonctionne  pas bien.

### Système de vagues
Pour le système de vague, un ensemble de fonctions définissemnt:
- Les HP par vague (une vague de 100HP spawnera 1à ennemis de 10Hp)
- Les HPPS par vague (10HPPS -> cette meme vague durrera 10s avec un ecart de 1s enctre chaque spawn)
- Les random_factor: pour chaque ennemi, ça correspond a la probabilité de le spawner 
- min spawn permet de forcer le spawn des boss a certaines vagues.

Voir [getWaveByIndex enemies.c:741](./src/enemies.c#L741) pour les fonctions matématiques de spawn

La fonction suivante (updateWaveSystem) décide a chaque frame si le délai est écoulé de quel ennemi spawn.

> Voir les commentaires dans le code pour plus de details.


#### Simulation
Pour aider dans l'équilibrage, nous avons crée un outil de simulation qui génère des vagues et enregistre le résultat dans un fichier .csv puis affiche le tout dans des graphiques.

Pour l'utiliser, installez matplotlib, pandas et executez:
```bash
./testing/test.sh nombre_de_vagues
```

> cet outil ne rentre pas en compte dans l'évaluation.