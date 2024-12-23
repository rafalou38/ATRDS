#include "tower.h"

// Tous les bouts de programmes avec des # comme #if,#else... sont des morceaux de programmes utilisés uniquement sous l'activation du parametre BULLETS_ON dans ./src/tower.h

#if BULLETS_ON
void updateTowers(Grid grid, EnemyPool ep, BulletPool *bp, float dt, GameStats *gs)
{
#else
void updateTowers(Grid grid, EnemyPool ep, float dt, GameStats *gs)
{
#endif
    for (int x = 0; x < grid.width; x++)
    {
        for (int y = 0; y < grid.height; y++) // Pour chaque cellules du terrain (de taille grid.width*grid.height)
        {
            if (grid.cells[x][y].hasTurret) // Si la case a une tourelle, on la mets a jour
            {
                int lvl = grid.cells[x][y].turret.lvl; // Son niveau
                if (grid.cells[x][y].turret.has_malus)
                {
                    grid.cells[x][y].turret.puissance_malus -= dt;
                    if (grid.cells[x][y].turret.puissance_malus <= 0)
                    {
                        grid.cells[x][y].turret.has_malus = false;
                    }
                }
                else
                {
                    grid.cells[x][y].turret.compteur += dt; // Son horloge interne / son compteur
                }
                if (grid.cells[x][y].turret.type == Mortier)
                {
                    if (grid.cells[x][y].turret.compteur_mortier <= 0 && grid.cells[x][y].turret.compteur_mortier != -1)
                    {
                        int w;
                        int h;
                        get_terminal_size(&w, &h);
                        fillBG(1, 1, w + 1, h + 1);
                        drawFullGrid(grid);
                        grid.cells[x][y].turret.compteur_mortier = -1;
                    }
                    else if (grid.cells[x][y].turret.compteur_mortier > 0)
                    {
                        grid.cells[x][y].turret.compteur_mortier -= dt;
                    }
                }
                if (!grid.cells[x][y].turret.has_malus)
                {
                    // Lorsque l'horloge interne dépasse la vitesse de rechargement (reload) de la tourelle, elle applique son effet :
                    if (grid.cells[x][y].turret.compteur >= grid.cells[x][y].turret.reload_delay[lvl])
                    {
                        int enemies_hit = 0;
                        if (grid.cells[x][y].turret.has_effect)
                        {
                            if (grid.cells[x][y].turret.effet == Money) // Génération d'argent (Banque)
                            {
                                gs->cash += grid.cells[x][y].turret.puissance_effet[lvl];
                                enemies_hit++;
                            }
                            else if (grid.cells[x][y].turret.effet == Stun) // Stun = Pétrification (Pétrificateur)
                            {
                                for (int i = 0; i < ep.count; i++)
                                {
                                    // Calcul du vecteur entre la tourelle et l’ennemi
                                    float dx = ep.enemies[i].grid_x - (x + 0.5); // on rajoute 0.5 afin que l'on calcule depuis le centre de la tourelle et pas depuis le coin supérieur gauche de la case.
                                    float dy = ep.enemies[i].grid_y - (y + 0.5);

                                    float d = sqrt(dx * dx + dy * dy);
                                    if (d <= grid.cells[x][y].turret.range_max[lvl] && d >= grid.cells[x][y].turret.range_min[lvl])
                                    {
                                        if (ep.enemies[i].type == ENEMY_BOSS_STUN && d <= ep.enemies[i].puissance_effet)
                                        {
                                            grid.cells[x][y].turret.malus = true;
                                            grid.cells[x][y].turret.puissance_malus = ep.enemies[i].puissance_effet;
                                        }
                                        else
                                        {
                                            grid.cells[x][y].turret.last_shot_dx = dx / d;
                                            grid.cells[x][y].turret.last_shot_dy = dy / d;
                                            ep.enemies[i].has_effect = true;
                                            ep.enemies[i].effet = Stun;
                                            ep.enemies[i].temps_rest = grid.cells[x][y].turret.puissance_effet[lvl];
                                            enemies_hit++;
                                            break;
                                        }
                                    }
                                }
                            }
                            else if (grid.cells[x][y].turret.effet == Slow) // Slow = Ralentissement (Freezer)
                            {
                                for (int i = 0; i < ep.count; i++)
                                {
                                    // Calcul du vecteur entre la tourelle et l’ennemi
                                    float dx = ep.enemies[i].grid_x - (x + 0.5);
                                    float dy = ep.enemies[i].grid_y - (y + 0.5);

                                    float d = sqrt(dx * dx + dy * dy);
                                    if (d <= grid.cells[x][y].turret.range_max[lvl] && d >= grid.cells[x][y].turret.range_min[lvl])
                                    {
                                        if (ep.enemies[i].type == ENEMY_BOSS_STUN && d <= ep.enemies[i].puissance_effet)
                                        {
                                            grid.cells[x][y].turret.has_malus = true;
                                            grid.cells[x][y].turret.puissance_malus = ep.enemies[i].puissance_effet;
                                        }
                                        else
                                        {
                                            grid.cells[x][y].turret.last_shot_dx = dx / d;
                                            grid.cells[x][y].turret.last_shot_dy = dy / d;
                                            ep.enemies[i].has_effect = true;
                                            ep.enemies[i].effet = Slow;
                                            ep.enemies[i].temps_rest = lvl + 1;
                                            ep.enemies[i].puissance_effet = grid.cells[x][y].turret.puissance_effet[lvl];
                                            enemies_hit++;
                                        }
                                    }
                                }
                            }
                            else if (grid.cells[x][y].turret.effet == Fire) // Fire = dégats sur la durée (Inferno)
                            {
                                for (int i = 0; i < ep.count; i++)
                                {
                                    // Calcul du vecteur entre la tourelle et l’ennemi
                                    float dx = ep.enemies[i].grid_x - (x + 0.5);
                                    float dy = ep.enemies[i].grid_y - (y + 0.5);
                                    float d = sqrt(dx * dx + dy * dy);

                                    if (d <= grid.cells[x][y].turret.range_max[lvl] && d >= grid.cells[x][y].turret.range_min[lvl])
                                    {
                                        if (ep.enemies[i].type == ENEMY_BOSS_STUN && d <= ep.enemies[i].puissance_effet)
                                        {
                                            grid.cells[x][y].turret.has_malus = true;
                                            grid.cells[x][y].turret.puissance_malus = ep.enemies[i].puissance_effet;
                                        }
                                        else
                                        {
                                            grid.cells[x][y].turret.last_shot_dx = dx / d;
                                            grid.cells[x][y].turret.last_shot_dy = dy / d;
                                            ep.enemies[i].has_effect = true;
                                            ep.enemies[i].effet = Fire;
                                            ep.enemies[i].temps_rest = lvl + 1;
                                            ep.enemies[i].temps_recharge = grid.cells[x][y].turret.puissance_effet[lvl];
                                            ep.enemies[i].last_hit = 10;
                                            enemies_hit++;
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            for (int i = 0; i < ep.count; i++)
                            {
                                // Calcul du vecteur entre la tourelle et l’ennemi
                                float dx = ep.enemies[i].grid_x - (x + 0.5);
                                float dy = ep.enemies[i].grid_y - (y + 0.5);
                                float d = sqrt(dx * dx + dy * dy);

                                // Test pour que l'ennemi soit dans la portée de la tourelle
                                if (d <= grid.cells[x][y].turret.range_max[lvl] && d >= grid.cells[x][y].turret.range_min[lvl])
                                {
                                    if (ep.enemies[i].type == ENEMY_BOSS_STUN && d <= ep.enemies[i].puissance_effet)
                                    {
                                        grid.cells[x][y].turret.has_malus = true;
                                        grid.cells[x][y].turret.puissance_malus = ep.enemies[i].puissance_effet;
                                    }
                                    else
                                    {
                                        float dx = ep.enemies[i].grid_x - x;
                                        float dy = ep.enemies[i].grid_y - y;
                                        float d = sqrt(dx * dx + dy * dy);

                                        grid.cells[x][y].turret.last_shot_dx = dx / d;
                                        grid.cells[x][y].turret.last_shot_dy = dy / d;
                                        int rapport_speed_damage = grid.cells[x][y].turret.compteur / grid.cells[x][y].turret.reload_delay[lvl];
                                        // Fonctionnement du splash damage (dégats de zone)
                                        if (grid.cells[x][y].turret.splash[lvl] != 0.0)
                                        {
                                            float d_min = grid.cells[x][y].turret.splash[lvl];
                                            for (int j = 0; j < ep.count; j++)
                                            {
                                                if (j != i)
                                                {
                                                    int d_enemy = sqrt(pow(ep.enemies[i].grid_x - ep.enemies[j].grid_x, 2) +
                                                                       pow(ep.enemies[i].grid_y - ep.enemies[j].grid_y, 2));
                                                    if (d_enemy < d_min)
                                                    {
                                                        ep.enemies[j].hp -= grid.cells[x][y].turret.damage[lvl] / 2;
                                                    }
                                                }
                                            }
                                            ep.enemies[i].hp -= grid.cells[x][y].turret.damage[lvl] * rapport_speed_damage;
                                            int w;
                                            int h;
                                            get_terminal_size(&w, &h);
                                            printf(COLOR_MORTIER_FIRING);
                                            drawRange(w, h, d_min, ep.enemies[i].grid_x, ep.enemies[i].grid_y, false);
                                            grid.cells[x][y].turret.compteur_mortier = 0.5;
                                            printf(RESET);
                                        }
                                        else // Tir standard
                                        {
#if BULLETS_ON
                                            bp->bullets[bp->count].hit = false;
                                            bp->bullets[bp->count].grid_x = x + 0.5 + (dx / d) / 4;
                                            bp->bullets[bp->count].grid_y = y + 0.5 + (dy / d) / 4;
                                            bp->bullets[bp->count].target = &(ep.enemies[i]);
                                            bp->bullets[bp->count].damage = grid.cells[x][y].turret.damage[lvl] * rapport_speed_damage;
                                            bp->count++;
#else
                                            // Changements des points de vie de l'ennemi touché, perdant ainsi le nombre de dégats infligés par l'attaque
                                            ep.enemies[i].hp -= grid.cells[x][y].turret.damage[lvl] * rapport_speed_damage;
#endif
                                        }
                                        // Fonctionnement du nombre d'ennemi pouvant être touché
                                        enemies_hit++;
                                        if (enemies_hit >= grid.cells[x][y].turret.nb_ennemi[lvl])
                                            break;
                                    }
                                }
                            }
                        }
                        if (enemies_hit == 0)
                        {
                            grid.cells[x][y].turret.in_range = false;
                        }
                        else
                        {
                            grid.cells[x][y].turret.in_range = true;
                        }
                        grid.cells[x][y].turret.compteur = 0;
                    }
                }
            }
        }
    }
}

#if BULLETS_ON
void defragBullets(BulletPool *bp)
{
    int right = 0;
    int left = -1;

    while (right < bp->count)
    {
        if (!bp->bullets[right].hit)
        {
            if (left != -1)
            {
                assert(left >= 0);
                // assert(left < bp->length);

                bp->bullets[left] = bp->bullets[right];
                left++;
            }
        }
        else if (left == -1)
        {
            left = right;
        }

        right++;
    }
    bp->count -= right - left;
    if (bp->count < 0)
        bp->count = 0;
}

void drawBullets(BulletPool bp)
{
    for (int i = 0; i < bp.count; i++)
    {
        int terminal_x = bp.bullets[i].grid_x * (CELL_WIDTH + GAP) + 3;
        int terminal_y = bp.bullets[i].grid_y * (CELL_HEIGHT + GAP / 2) + 2;
        move_to(terminal_x, terminal_y);

        printf("⌾");
    }
}
void updateBullets(BulletPool *bp, float dt)
{
    float defrag_needed = false;
    for (int i = 0; i < bp->count; i++)
    {
        if (bp->bullets[i].hit)
            continue;

        float dx = bp->bullets[i].target->grid_x - bp->bullets[i].grid_x;
        float dy = bp->bullets[i].target->grid_y - bp->bullets[i].grid_y;
        float d = sqrt(dx * dx + dy * dy);
        bp->bullets[i].grid_x += (dx / d) * 10 * dt;
        bp->bullets[i].grid_y += (dy / d) * 10 * dt;

        if (d <= 0.1)
        {
            bp->bullets[i].target->hp -= bp->bullets[i].damage;
            bp->bullets[i].hit = true;
            defrag_needed = true;
        }
    }
    if (defrag_needed)
        defragBullets(bp);
};

#endif

// Prix des tourelles
int getTurretPrice(enum TurretType type, int level)
{
    if (type == Sniper)
    {
        if (level == 0)
        {
            return 10;
        }
        if (level == 1)
        {
            return 40;
        }
        if (level == 2)
        {
            return 80;
        }
    }
    if (type == Inferno)
    {
        if (level == 0)
        {
            return 20;
        }
        if (level == 1)
        {
            return 60;
        }
    }
    if (type == Mortier)
    {
        if (level == 0)
        {
            return 35;
        }
        if (level == 1)
        {
            return 80;
        }
    }
    if (type == Gatling)
    {
        if (level == 0)
        {
            return 35;
        }
        if (level == 1)
        {
            return 100;
        }
    }
    if (type == Freezer)
    {
        if (level == 0)
        {
            return 25;
        }
        if (level == 1)
        {
            return 70;
        }
    }
    if (type == Petrificateur)
    {
        if (level == 0)
        {
            return 10;
        }
        if (level == 1)
        {
            return 40;
        }
    }
    if (type == Banque)
    {
        if (level == 0)
        {
            return 100;
        }
        if (level == 1)
        {
            return 200;
        }
    }

    return -1;
}

// Definition de toutes les caractéristiques de chaque tourelle
struct Turret getTurretStruct(enum TurretType type)
{
    struct Turret tur;
    tur.in_range = false;
    tur.has_malus = false;
    tur.compteur_mortier = -1;
    if (type == Sniper)
    {
        tur.type = Sniper;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 0;
        tur.range_min[1] = 0;
        tur.range_min[2] = 0;
        tur.range_max[0] = 2.5;
        tur.range_max[1] = 4.5;
        tur.range_max[2] = 100;
        tur.damage[0] = 1;
        tur.damage[1] = 1.3;
        tur.damage[2] = 1.6;
        tur.reload_delay[0] = 1;
        tur.reload_delay[1] = 0.75;
        tur.reload_delay[2] = 0.75;
        tur.splash[0] = 0.0;
        tur.splash[1] = 0.0;
        tur.splash[2] = 0.0;
        tur.nb_ennemi[0] = 1;
        tur.nb_ennemi[1] = 1;
        tur.nb_ennemi[2] = 1;
        tur.has_effect = false;
    }
    else if (type == Inferno)
    {
        tur.type = Inferno;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 0;
        tur.range_min[1] = 0;
        tur.range_max[0] = 1.5;
        tur.range_max[1] = 2.5;
        tur.damage[0] = 0.5;
        tur.damage[1] = 0.5;
        tur.reload_delay[0] = 1;
        tur.reload_delay[1] = 1;
        tur.splash[0] = 0.0;
        tur.splash[1] = 0.0;
        tur.nb_ennemi[0] = 4;
        tur.nb_ennemi[1] = 8;
        tur.has_effect = true;
        tur.effet = Fire;
        tur.puissance_effet[0] = 0.5;
        tur.puissance_effet[1] = 0.5;
    }
    else if (type == Mortier)
    {
        tur.type = Mortier;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 3;
        tur.range_min[1] = 2.5;
        tur.range_max[0] = 5.5;
        tur.range_max[1] = 6.5;
        tur.damage[0] = 2;   // 4/4 -> 1dps
        tur.damage[1] = 2.5; // 5/3 -> 1.42dps
        tur.reload_delay[0] = 4;
        tur.reload_delay[1] = 3.5;
        tur.splash[0] = 1.0;
        tur.splash[1] = 1.5;
        tur.nb_ennemi[0] = 1;
        tur.nb_ennemi[1] = 1;
        tur.has_effect = false;
    }
    else if (type == Gatling)
    {
        tur.type = Gatling;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 0;
        tur.range_min[1] = 0;
        tur.range_max[0] = 1.5;
        tur.range_max[1] = 2;
        tur.damage[0] = 0.2;
        tur.damage[1] = 0.3;
        tur.reload_delay[0] = 0.05;
        tur.reload_delay[1] = 0.03;
        tur.splash[0] = 0;
        tur.splash[1] = 0;
        tur.nb_ennemi[0] = 1;
        tur.nb_ennemi[1] = 1;
        tur.has_effect = false;
    }
    else if (type == Petrificateur)
    {
        tur.type = Petrificateur;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 0;
        tur.range_min[1] = 0;
        tur.range_max[0] = 1.5;
        tur.range_max[1] = 2;
        tur.damage[0] = 0;
        tur.damage[1] = 0;
        tur.reload_delay[0] = 2;
        tur.reload_delay[1] = 1.5;
        tur.splash[0] = 0;
        tur.splash[1] = 0;
        tur.nb_ennemi[0] = 1;
        tur.nb_ennemi[1] = 1;
        tur.has_effect = true;
        tur.effet = Stun;
        tur.puissance_effet[0] = 1;
        tur.puissance_effet[1] = 1;
    }
    else if (type == Freezer)
    {
        tur.type = Freezer;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.range_min[0] = 0;
        tur.range_min[1] = 0;
        tur.range_max[0] = 1.5;
        tur.range_max[1] = 2.5;
        tur.damage[0] = 0;
        tur.damage[1] = 0;
        tur.reload_delay[0] = 3;
        tur.reload_delay[1] = 3;
        tur.splash[0] = 0;
        tur.splash[1] = 0;
        tur.nb_ennemi[0] = 100;
        tur.nb_ennemi[1] = 100;
        tur.has_effect = true;
        tur.effet = Slow;
        tur.puissance_effet[0] = 0.90;
        tur.puissance_effet[1] = 0.75;
    }
    else if (type == Banque)
    {
        tur.type = Banque;
        tur.lvl = 0;
        tur.compteur = 0;
        tur.reload_delay[0] = 5;
        tur.reload_delay[1] = 10;
        tur.has_effect = true;
        tur.effet = Money;
        tur.puissance_effet[0] = 2;
        tur.puissance_effet[1] = 6;
    }
    return tur;
}

void showSaveScreen(int ligne)
{
    int terminal_width, terminal_height;
    get_terminal_size(&terminal_width, &terminal_height);

    int width = 80;  // terminal_width/2;
    int height = 30; // terminal_height/2;
    assert(width < terminal_width);
    assert(height < terminal_height);

    int x0 = terminal_width / 2.0f - width / 2.0f;
    int y0 = terminal_height / 2.0f - height / 2.0f + 1;

    printf(RESET);
    printf(COLOR_STANDARD_BG);

    // Bordures et coeur de la fenêtre
    for (int y = 0; y < height; y++)
    {
        move_to(x0, y0 + y);

        for (int x = 0; x < width; x++)
        {
            printf(RESET);
            if (y == ligne)
                printf("\033[38;5;221m");
            if (x == 0 || x == width - 1)
                printf("█");
            else if (y == 0)

                printf("▀");
            else if (y == height - 1)
                printf("▄");
            else
                printf(" ");
        }
    }
    move_to(x0 + 1, y0 + 1);
    printf("\"Enter\" pour load, \"Espace\" pour save, \"S\" pour fermer");
    move_to(x0 + 1, y0 + 2);
    printf("Si vous quittez avec \"Q\", votre partie sera sauvegardé en 0");
    for (int y = 3; y < 10; y++)
    {
        move_to(x0 + 1, y0 + y);
        printf("save_%d", y - 3);
    }
}

// Fonction qui pause le jeu et ouvre le panneau de séléction
void showTowerSelection(int ligne, bool hasTurret, struct Turret selectedTurret)
{
    int terminal_width, terminal_height;
    get_terminal_size(&terminal_width, &terminal_height);

    int width = 80;  // terminal_width/2;
    int height = 30; // terminal_height/2;
    assert(width < terminal_width);
    assert(height < terminal_height);

    int x0 = terminal_width / 2.0f - width / 2.0f;
    int y0 = terminal_height / 2.0f - height / 2.0f + 1;

    printf(RESET);
    printf(COLOR_STANDARD_BG);

    // Bordures et coeur de la fenêtre
    for (int y = 0; y < height; y++)
    {
        move_to(x0, y0 + y);

        for (int x = 0; x < width; x++)
        {
            printf(RESET);
            if (y == ligne)
                printf("\033[38;5;221m");
            if (x == 0 || x == width - 1)
                printf("█");
            else if (y == 0)

                printf("▀");
            else if (y == height - 1)
                printf("▄");
            else
                printf(" ");
        }
    }

    // Test pour voir si une tourelle est présente, dans ce cas proposer l'amélioration ou la vente
    if (hasTurret)
    {
        int next_price = getTurretPrice(selectedTurret.type, selectedTurret.lvl + 1);

        if (next_price != -1)
        {
            move_to(x0 + 1, y0 + 1);
            printf(" 🛠️ Upgrade");

            move_to(x0 + 1 + width - 8, y0 + 1);
            printf(COLOR_YELLOW "-% 3d €" RESET, next_price);
        }

        move_to(x0 + 1, y0 + 2);
        printf(" 💰 Sell");

        move_to(x0 + 1 + width - 8, y0 + 2);
        printf(COLOR_GREEN "+% 3d €" RESET, (int)(getTurretPrice(selectedTurret.type, selectedTurret.lvl) * 0.8));
    }
    // Sinon, proposer la construction des autres tourelles
    else
    {
        move_to(x0 + 1, y0 + 1);
        printf(" 🔫 Sniper");
        move_to(x0 + 1 + width - 10, y0 + 1);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Sniper, 0));

        move_to(x0 + 1, y0 + 2);
        printf(" 🔥 Inferno");
        move_to(x0 + 1 + width - 10, y0 + 2);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Inferno, 0));

        move_to(x0 + 1, y0 + 3);
        printf(" 🔬 Mortier ");
        move_to(x0 + 1 + width - 10, y0 + 3);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Mortier, 0));

        move_to(x0 + 1, y0 + 4);
        printf(" ⚙️  Gatling ");
        move_to(x0 + 1 + width - 10, y0 + 4);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Gatling, 0));

        move_to(x0 + 1, y0 + 5);
        printf(" ⏳ Petrificateur ");
        move_to(x0 + 1 + width - 10, y0 + 5);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Petrificateur, 0));

        move_to(x0 + 1, y0 + 6);
        printf(" ❄️  Freezer ");
        move_to(x0 + 1 + width - 10, y0 + 6);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Freezer, 0));

        move_to(x0 + 1, y0 + 7);
        printf(" 💲 Banque ");
        move_to(x0 + 1 + width - 10, y0 + 7);
        printf(COLOR_YELLOW "-% 3d €" RESET, getTurretPrice(Banque, 0));
    }
    printf(RESET);
}