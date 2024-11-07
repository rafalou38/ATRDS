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
      - [Allocations dynamiques](#allocations-dynamiques)
        - [Sécurité](#sécurité)
        - [Grille](#grille)
      - [Affichage](#affichage)
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
> Il manque également toute la partie simulation ainsi que les scripts.

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
Si tu est dans vscode ou sur github, tu dois pouvoir click/ctrl+click sur les liens pour aller directement au fichier et a la ligne correspondante.
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
        - Il ses PV quand un ennemi atteint la fin du chemin
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
Les fichiers `.c` contiennent le code du jeu et son inclus au moment de la compilation.
Exemple:
> **grid.h** contient la définition de la fonction, valeur de retour et paramètres, il est importé avec les #include, et expose ses fonctions pour qu'elles puissent être utilisées par d'autres fichiers.
```c
void fillBG(int xmin, int ymin, int xmax, int ymax);
```
> **grid.c** le code de la fonction est automatiquement inclut a la compilation par la présence de:
> `gcc -o atrds.out -std=c99 src/main.c [src/grid.c] src/shared.c src/enemies.c src/tower.c -lm`
> il n'est pas inclus par les #include, c a uniquement besoin de la "signature" de la fonction pour verifier la validité programme.
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

#### Allocations dynamiques

##### Sécurité

##### Grille

#### Affichage

### Simulation
