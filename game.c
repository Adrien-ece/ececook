/*#include "game.h"
#include "generation.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// Variables globales
Niveau niveaux[MAX_NIVEAUX];

char pseudoJoueur1[50];
char pseudoJoueur2[50];
Rectangle zonesInterdites[] = {
        {0, 0, 263, 138},
        {263, 0, 307, 99},
        {308, 0, 381, 130},
        {382, 0, 406, 108},
        {406, 0, 481, 131},
        {481, 0, 521, 148},
        {521, 0, 679, 106},
        {562, 106, 679, 157},
        {599, 157, 679, 268},
        {415, 295, 680, 420},
        {192, 300, 259, 440},
        {85, 291, 193, 440},
        {0, 252, 86, 440},
        {0, 211, 85, 252},
        {0, 175, 93, 211},
        {0, 138, 102, 175},
        {237, 159, 448, 260},
};
ZoneMessage zonesMessages[] = {
        {{93, 181, 110, 210}, "Prendre poisson cru"},
        {{87, 215, 100, 251}, "Prendre du fromage"},
        {{102, 140, 112, 175}, "Prendre un legume"},
        {{220, 199, 289, 274}, "Decouper un aliment"},
        {{222, 155, 298, 198}, "Poser un aliment"},
        {{307, 146, 382, 171}, "Cuire un aliment"},
        {{303, 247, 384, 279}, "Cuire un aliment"},
        {{521, 106, 539, 117}, "Jeter aliment"},
        {{490, 279, 643, 297}, "Prendre assiette"},
        {{259, 388, 416, 440}, "Livrer un plat"}
};
int niveauActuel = 0;

// Fonction pour afficher le minuteur en haut à droite de l'écran avec différentes couleurs en fonction du temps restant
void afficher_minuteur(int tempsRestant) {
    if (tempsRestant >= 91) {
        textprintf_right_ex(screen, font, LARGEUR_ECRAN - 10, 10, makecol(0, 255, 0), -1, "%d s", tempsRestant);
    } else if (tempsRestant >= 61) {
        textprintf_right_ex(screen, font, LARGEUR_ECRAN - 10, 10, makecol(255, 255, 0), -1, "%d s", tempsRestant);
    } else if (tempsRestant >= 31) {
        textprintf_right_ex(screen, font, LARGEUR_ECRAN - 10, 10, makecol(255, 128, 0), -1, "%d s", tempsRestant);
    } else if (tempsRestant >= 10) {
        textprintf_right_ex(screen, font, LARGEUR_ECRAN - 10, 10, makecol(255, 0, 0), -1, "%d s", tempsRestant);
    }
}
void reinitialiserCommandes(int niveau) {
    for (int i = 0; i < 2; i++) {
        niveaux[niveau].commandes[i].itemIndex = -1;  // Réinitialisez l'index des commandes
        if (niveaux[niveau].commandes[i].image) {
            destroy_bitmap(niveaux[niveau].commandes[i].image);  // Détruisez l'image si elle existe
            niveaux[niveau].commandes[i].image = NULL;
        }
    }
}


// Initialisation de l'inventaire
void initialiserInventaire(Inventaire *inventaire) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        inventaire->items[i] = 0;
        strcpy(inventaire->itemNames[i], "");  // Noms vides au départ
        inventaire->pileItemsPris[i] = -1;  // Pile vide
    }
    inventaire->sommetPile = -1;  // Pile vide
    inventaire->itemPose = -1;  // Aucun item posé au départ
    inventaire->posItemX = -1;  // Position X initiale de l'item posé
    inventaire->posItemY = -1;  // Position Y initiale de l'item posé
}

// Ajouter un item à l'inventaire et à la pile
void ajouterItem(Inventaire *inventaire, int indexItem) {
    inventaire->items[indexItem]++;
    inventaire->sommetPile++;
    inventaire->pileItemsPris[inventaire->sommetPile] = indexItem;
}

// Jeter le dernier item pris
void jeterDernierItem(Inventaire *inventaire) {
    // Vérifie si la pile n'est pas vide
    if (inventaire->sommetPile >= 0) {
        int dernierItemIndex = inventaire->pileItemsPris[inventaire->sommetPile];  // Récupère l'index du dernier item ajouté à la pile

        // Vérifie si l'item à cet index a une quantité supérieure à 0 avant de le jeter
        if (inventaire->items[dernierItemIndex] > 0) {
            inventaire->items[dernierItemIndex]--;  // Décrémente la quantité de cet item
            printf("Item '%s' jete de l'inventaire.\n", inventaire->itemNames[dernierItemIndex]);

            // Retire cet item de la pile
            inventaire->sommetPile--;
        } else {
            printf("Aucun item a jeter pour l'index spécifie.\n");
        }
    } else {
        printf("Aucun item a jeter, la pile est vide.\n");
    }
}

void dessiner_jauge(int x, int y, int largeur, int hauteur, double pourcentage) {
    // Dessiner le fond de la jauge (en gris)
    rectfill(screen, x, y, x + largeur, y + hauteur, makecol(128, 128, 128));

    // Calculer la largeur de la jauge en fonction du pourcentage de remplissage
    int largeurRemplie = (int)(largeur * pourcentage);

    // Dessiner la jauge remplie (en vert)
    rectfill(screen, x, y, x + largeurRemplie, y + hauteur, makecol(0, 255, 0));
}

void verifierLivraison(Inventaire *inventaire, int itemIndex, int *score, int *nombreLivraisons, int tempsRestant) {
    int typeDePlat = 0;
    if (itemIndex == 8) {  // Poisson à la sauce tomate
        typeDePlat = 10;
    } else if (itemIndex == 9) {  // Ceviche
        typeDePlat = 5;
    }
    else if (itemIndex == 16){
        typeDePlat == 20;
    }
    else if (itemIndex == 17){
        typeDePlat == 10;
    }

    if (typeDePlat > 0) {
        for (int i = 0; i < 2; i++) {
            if (niveaux[niveauActuel].commandes[i].itemIndex == itemIndex && inventaire->items[itemIndex] > 0) {
                inventaire->items[itemIndex]--;  // Retire l'item de l'inventaire
                printf("Commande livree : %s\n", inventaire->itemNames[itemIndex]);

                // Met à jour le score du joueur
                (*nombreLivraisons)++;
                *score += typeDePlat * tempsRestant * (*nombreLivraisons);

                // Générer une nouvelle commande
                niveaux[niveauActuel].commandes[i].itemIndex = (rand() % 2) + 8;  // Nouvelle commande aléatoire (soit poisson à la sauce tomate soit ceviche)
                // Mettre à jour l'image correspondante
                switch (niveaux[niveauActuel].commandes[i].itemIndex) {
                    case 8:
                        niveaux[niveauActuel].commandes[i].image = charger_image("../saumon_tomate.bmp");
                        break;
                    case 9:
                        niveaux[niveauActuel].commandes[i].image = charger_image("../ceviche_tomate.bmp");
                        break;
                    case 16 :
                        niveaux[niveauActuel].commandes[i].image = charger_image("../sandwich.bmp");
                        break;
                    case 17 :
                        niveaux[niveauActuel].commandes[i].image = charger_image("../salade_exo.bmp");
                        break;


                }
                break;
            }
        }
    }
}

void initialiserCommandes() {
    niveaux[niveauActuel].commandes[0].itemIndex = 8;  // Poisson à la sauce tomate
    niveaux[niveauActuel].commandes[0].image = charger_image("../saumon_tomate.bmp");
    niveaux[niveauActuel].commandes[1].itemIndex = 9;  // Ceviche
    niveaux[niveauActuel].commandes[1].image = charger_image("../ceviche_tomate.bmp");
}

void afficherCommandes() {
    for (int i = 0; i < 2; i++) {
        draw_sprite(screen, niveaux[niveauActuel].commandes[i].image, 10, HAUTEUR_ECRAN - (i + 1) * 50);
    }
}

void afficherScores() {
    textprintf_ex(screen, font, 10, 10, makecol(0, 0, 0), -1, "Score Joueur 1: %d", niveaux[niveauActuel].scoreJoueur1);
    textprintf_ex(screen, font, 10, 30, makecol(255, 255, 255), -1, "Score Joueur 2: %d", niveaux[niveauActuel].scoreJoueur2);
}

void chargerItems(Inventaire *inventaire) {
    FILE *file = fopen("../items.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Erreur a l'ouverture du fichier items.txt\n");
        return;
    }

    int id;
    char itemName[50];
    while (fscanf(file, "%d=%49[^\n]", &id, itemName) == 2) {
        if (id > 0 && id <= MAX_ITEMS) {
            strcpy(inventaire->itemNames[id - 1], itemName);  // Copie le nom de l'item
            inventaire->items[id - 1] = 0;  // Initialise la quantité à 0
        }
    }
    fclose(file);
}

void saisirPseudo(char pseudo[], int joueur) {
    clear_keybuf();
    clear_bitmap(screen);

    textprintf_centre_ex(screen, font, LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2 - 20, makecol(255, 255, 255), -1, "Joueur %d, veuillez saisir votre pseudo:", joueur);
    textprintf_centre_ex(screen, font, LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2 + 20, makecol(255, 255, 255), -1, "(Appuyez sur ENTER pour valider)");

    int key;
    int index = 0;
    while (1) {
        if (keypressed()) {
            key = readkey() & 0xff;
            if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9') || key == ' ') {
                pseudo[index] = key;
                index++;
                pseudo[index] = '\0';
            } else if (key == '\b' && index > 0) {
                index--;
                pseudo[index] = '\0';
            } else if (key == '\r') {
                break;
            }
            clear_bitmap(screen);
            textprintf_centre_ex(screen, font, LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2, makecol(255, 255, 255), -1, "Joueur %d: %s_", joueur, pseudo);
        }
    }
}

void jouerPartie() {
    // Charger la musique de fond
    SAMPLE* musique_fond = charger_musique("background_music.wav");
    if (!musique_fond) {
        return;
    }

    BITMAP *imagePoissonCru = charger_image("../poisson_cru.bmp");
    if(!imagePoissonCru){
        return;
    }

    BITMAP *imagePoissonCuit = charger_image("../poisson_cuit.bmp");
    if(!imagePoissonCuit){
        return;
    }

    BITMAP *imageLegume = charger_image("../tomate.bmp");
    if(!imageLegume){
        return;
    }

    BITMAP *imageLegumeDecoupe = charger_image("../tomate_cut.bmp");
    if(!imageLegumeDecoupe){
        return;
    }

    BITMAP *imageAssiette = charger_image("../assiette.bmp");
    if(!imageAssiette){
        return;
    }

    BITMAP *imagePoissonTomate = charger_image("../saumon_tomate.bmp");
    if(!imagePoissonTomate){
        return;
    }

    BITMAP *imageCeviche = charger_image("../ceviche_tomate.bmp");
    if(!imageCeviche){
        return;
    }

    // Charger les items pour les deux joueurs
    chargerItems(&niveaux[0].inventaireJoueur1);
    chargerItems(&niveaux[0].inventaireJoueur2);

    initialiserCommandes();

    int key_c_pressed1 = 1; // Flag pour surveiller si la touche C a été pressée
    int key_l_pressed2 = 1; // Idem pour le joueur 2
    int key_v_pressed1 = 1;
    int key_m_pressed2 = 1;

    initialiserInventaire(&niveaux[0].inventaireJoueur1);  // Charge les items pour le joueur 1
    initialiserInventaire(&niveaux[0].inventaireJoueur2);  // Charge les items pour le joueur 2

    // Saisir les pseudonymes des joueurs
    saisirPseudo(pseudoJoueur1, 1);
    saisirPseudo(pseudoJoueur2, 2);

    stop_sample(musique_fond);
    play_sample(musique_fond, 255, 128, 1000, 1);
    // Charger l'image de fond pour le menu
    BITMAP* grille1 = charger_image("../grilleniveau1.bmp");
    if (!grille1) {
        return;
    }

    // Position initiale de la balle1
    int balleX1 = 300;
    int balleY1 = 300;

    // Position initiale de la balle2
    int balleX2 = 520;
    int balleY2 = 200;

    // Temps écoulé
    int tempsDebut = (int)time(NULL); // Temps de début en secondes
    int tempsTotal = 120; // Durée de la partie en secondes
    int tempsRestant = tempsTotal; // Temps restant en secondes

    // Boucle principale pour le jeu
    while (!key[KEY_ESC]) {
        // Affichage de la souris pendant le jeu
        show_mouse(screen);

        // Efface l'écran
        clear_bitmap(screen);

        // Dessine l'image de fond
        blit(grille1, screen, 0, 0, 0, 0, grille1->w, grille1->h);

        // Dessine la balle1 avec le pseudo du joueur 1 au-dessus
        circlefill(screen, balleX1, balleY1, TAILLE_BALLE / 2, makecol(255, 0, 0));
        textout_centre_ex(screen, font, pseudoJoueur1, balleX1, balleY1 - 20, makecol(255, 255, 255), -1);

        // Dessine la balle2 avec le pseudo du joueur 2 au-dessus
        circlefill(screen, balleX2, balleY2, TAILLE_BALLE / 2, makecol(0, 255, 0));
        textout_centre_ex(screen, font, pseudoJoueur2, balleX2, balleY2 - 20, makecol(255, 255, 255), -1);

        // Dessine les scores des joueurs
        afficherScores();

        // Vérifier si le score total atteint 100
        int scoreTotal = niveaux[0].scoreJoueur1 + niveaux[0].scoreJoueur2;
        if (scoreTotal >= 1000) {
            clear_bitmap(screen);
            textprintf_centre_ex(screen, font, LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2, makecol(0, 255, 0), -1, "Niveau 1 validé !");
            rest(3000);
            reinitialiserCommandes(1);// Réinitialisez les commandes pour le niveau 2
            stop_sample(musique_fond);
            jouerNiveau2();  // Passez au niveau 2
            return;
        }

        // Sauvegarder les positions précédentes des balles
        int ancienX1 = balleX1;
        int ancienY1 = balleY1;
        int ancienX2 = balleX2;
        int ancienY2 = balleY2;

        // Déplacement de la balle1
        int nouvellePosX1 = balleX1;
        int nouvellePosY1 = balleY1;
        if (key[KEY_W]) nouvellePosY1--;
        if (key[KEY_S]) nouvellePosY1++;
        if (key[KEY_A]) nouvellePosX1--;
        if (key[KEY_D]) nouvellePosX1++;

        // Vérifier si les nouvelles coordonnées de la balle1 tombent dans une zone interdite
        int collision1 = 0;
        for (int i = 0; i < sizeof(zonesInterdites) / sizeof(zonesInterdites[0]); i++) {
            if (nouvellePosX1 >= zonesInterdites[i].x1 && nouvellePosX1 <= zonesInterdites[i].x2 &&
                nouvellePosY1 >= zonesInterdites[i].y1 && nouvellePosY1 <= zonesInterdites[i].y2) {
                collision1 = 1;
                break;
            }
        }

        // Vérifier si la nouvelle position de la balle1 entre en collision avec la balle2
        if (!collision1 && abs(nouvellePosX1 - balleX2) < TAILLE_BALLE && abs(nouvellePosY1 - balleY2) < TAILLE_BALLE) {
            collision1 = 1;
        }

        // Si aucune collision, déplacer la balle1
        if (!collision1) {
            balleX1 = nouvellePosX1;
            balleY1 = nouvellePosY1;
        }

        // Intéraction dans la zone pour prendre l'assiette pour le joueur 1
        if(balleX1 >= 490 && balleX1 <= 643 && balleY1 >= 279 && balleY1 <= 297) {
            if (key[KEY_C] && !key_c_pressed1) {
                ajouterItem(&niveaux[0].inventaireJoueur1, 7);
                printf("Joueur 1 : Assiette ajoutee a l'inventaire\n");
                key_c_pressed1 = 1;
            }
        }

        // Dessine l'image de l'assiette si le joueur a l'assiette dans son inventaire
        if (niveaux[0].inventaireJoueur1.items[7] > 0) {
            draw_sprite(screen, imageAssiette, balleX1 + 20, balleY1 - 20);
        }

        // Interaction spécifique dans la zone pour le poisson cru
        if (balleX1 >= 93 && balleX1 <= 110 && balleY1 >= 181 && balleY1 <= 210) {
            // Gère la prise du poisson cru lorsque la touche C est pressée et que la touche n'était pas déjà pressée
            if (key[KEY_C] && !key_c_pressed1) {
                ajouterItem(&niveaux[0].inventaireJoueur1, 5);
                printf("Joueur 1 : Poisson cru ajoute a l'inventaire: %s\n", niveaux[0].inventaireJoueur1.itemNames[5]);
                key_c_pressed1 = 1;  // Empêche la répétition jusqu'à ce que la touche soit relâchée
            }
        }

        // Dessine l'image du poisson si le joueur a un poisson cru
        if (niveaux[0].inventaireJoueur1.items[5] > 0) {  // Supposons que l'index 6 correspond au poisson cru
            draw_sprite(screen, imagePoissonCru, balleX1 + 20, balleY1 - 20); // Ajuste la position relative au personnage
        }

        if (balleX1 >= 102 && balleX1 <= 112 && balleY1 >= 140 && balleX1 <= 175) {
            // Gère la prise du legume lorsque la touche C est pressée et que la touche n'était pas déjà pressée
            if (key[KEY_C] && !key_c_pressed1) {
                ajouterItem(&niveaux[0].inventaireJoueur1, 0);
                printf("Joueur 1 : Legume ajoute a l'inventaire");
                key_c_pressed1 = 1;
            }
        }

        // Dessine l'image du legume si le joueur a un legume
        if (niveaux[0].inventaireJoueur1.items[0] > 0) {
            draw_sprite(screen, imageLegume, balleX1 + 20, balleY1 - 20); // Ajuste la position relative au personnage
        }

        // Interaction pour cuire le poisson pour le Joueur 1
        if ((balleX1 >= 307 && balleX1 <= 382 && balleY1 >= 146 && balleY1 <= 171) ||
            (balleX1 >= 303 && balleX1 <= 384 && balleY1 >= 247 && balleY1 <= 279)) {
            if (key[KEY_V] && niveaux[0].inventaireJoueur1.items[5] > 0 && !niveaux[0].cuisson_commencee_j1) {
                niveaux[0].temps_debut_cuisson_j1 = time(NULL);  // Début du timing de cuisson
                niveaux[0].cuisson_commencee_j1 = 1;
            }
        }

        // Vérifier si la cuisson est terminée pour le Joueur 1
        if (niveaux[0].cuisson_commencee_j1 && (time(NULL) - niveaux[0].temps_debut_cuisson_j1) >= 3) {
            if (niveaux[0].inventaireJoueur1.items[5] > 0) {  // Assurez-vous qu'il y a des poissons crus
                niveaux[0].inventaireJoueur1.items[5]--;
                ajouterItem(&niveaux[0].inventaireJoueur1, 6);  // Index 6 pour 'poisson cuit'
                niveaux[0].cuisson_commencee_j1 = 0;
                printf("Joueur 1 : Poisson cuit ajoute a l'inventaire\n");
            } else {
                niveaux[0].cuisson_commencee_j1 = 0;  // Réinitialiser si aucune cuisson n'est possible
                printf("Joueur 1 : Aucun poisson cru a cuire\n");
            }
        }

        // Dessine l'image du poisson si le joueur a un poisson cuit
        if (niveaux[0].inventaireJoueur1.items[6] > 0) {  // Supposons que l'index 6 correspond au poisson cuit
            draw_sprite(screen, imagePoissonCuit, balleX1 + 20, balleY1 - 20); // Ajuste la position relative au personnage
        }

        // Intéraction pour la découpe du legume pour le joueur1
        if (balleX1 >= 220 && balleX1 <= 289 && balleY1 >= 199 && balleY1 <= 274) {
            if (key[KEY_V] && niveaux[0].inventaireJoueur1.items[0] > 0 && !niveaux[0].decoupe_commencee_j1) {
                niveaux[0].temps_debut_decoupe_j1 = time(NULL);
                niveaux[0].decoupe_commencee_j1 = 1;
            }
        }

        if (niveaux[0].decoupe_commencee_j1 && (time(NULL) - niveaux[0].temps_debut_decoupe_j1) >= 2) {
            if (niveaux[0].inventaireJoueur1.items[0] > 0) {
                niveaux[0].inventaireJoueur1.items[0]--;
                ajouterItem(&niveaux[0].inventaireJoueur1, 1);
                niveaux[0].decoupe_commencee_j1 = 1;
                printf("Joueur 1 : Tomate decoupee ajoute a l'inventaire\n");
            } else {
                niveaux[0].decoupe_commencee_j1 = 0;
                printf("Joueur 1 : Aucun legume a decouper\n");
            }
        }

        // Dessine l'image du legume decoupe si le joueur a un legume decoupe
        if (niveaux[0].inventaireJoueur1.items[1] > 0) {  // Supposons que l'index 1 correspond au legume decoupe
            draw_sprite(screen, imageLegumeDecoupe, balleX1 + 20, balleY1 - 20); // Ajuste la position relative au personnage
        }

        // Intéraction pour avoir un poisson a la sauce tomate
        if (niveaux[0].inventaireJoueur1.items[1] > 0 && niveaux[0].inventaireJoueur1.items[6] > 0) {
            niveaux[0].inventaireJoueur1.items[1]--;
            niveaux[0].inventaireJoueur1.items[6]--;
            ajouterItem(&niveaux[0].inventaireJoueur1, 8);
            printf("Joueur 1 : Poisson a la sauce tomate ajoute a l'inventaire\n");
        }

        // Dessine l'image du poisson cuit a la sauce tomate si le joueur 1 en a un
        if (niveaux[0].inventaireJoueur1.items[8] > 0) {
            draw_sprite(screen, imagePoissonTomate, balleX1 + 20, balleY1 - 20); // Ajuste la position relative au personnage
        }

        // Intéraction pour avoir un ceviche si le joueur 1 en a un
        if (niveaux[0].inventaireJoueur1.items[1] > 0 && niveaux[0].inventaireJoueur1.items[5] > 0) {
            niveaux[0].inventaireJoueur1.items[1]--;
            niveaux[0].inventaireJoueur1.items[5]--;
            ajouterItem(&niveaux[0].inventaireJoueur1, 9);
            printf("Joueur 1 : Ceviche a la tomate ajoute a l'inventaire\n");
        }

        // Dessine l'image du ceviche si le joueur 1 en a un
        if (niveaux[0].inventaireJoueur1.items[9] > 0) {
            draw_sprite(screen, imageCeviche, balleX1 + 20, balleY1 - 20);
        }

        // Intéraction pour livrer les plats pour le joueur 1
        if (balleX1 >= 259 && balleX1 <= 416 && balleY1 >= 388 && balleY1 <= 440) {
            if (key[KEY_V] && !key_v_pressed1) {
                if (niveaux[0].inventaireJoueur1.items[8] > 0) {
                    verifierLivraison(&niveaux[0].inventaireJoueur1, 8, &niveaux[0].scoreJoueur1, &niveaux[0].nombreLivraisonsJoueur1, tempsRestant);  // Vérifie la livraison pour le poisson à la sauce tomate
                }
                if (niveaux[0].inventaireJoueur1.items[9] > 0) {
                    verifierLivraison(&niveaux[0].inventaireJoueur1, 9, &niveaux[0].scoreJoueur1, &niveaux[0].nombreLivraisonsJoueur1, tempsRestant);  // Vérifie la livraison pour le ceviche
                }
                key_v_pressed1 = 1;
            }
        }

        // Interaction pour poser ou reprendre un item pour le joueur 1
        if (balleX1 >= 222 && balleX1 <= 298 && balleY1 >= 155 && balleY1 <= 198) {
            if (key[KEY_C] && !key_c_pressed1) {
                if (niveaux[0].inventaireJoueur1.itemPose == -1 && niveaux[0].inventaireJoueur1.sommetPile >= 0) {
                    // Poser l'item
                    int indexItem = niveaux[0].inventaireJoueur1.pileItemsPris[niveaux[0].inventaireJoueur1.sommetPile];
                    niveaux[0].inventaireJoueur1.items[indexItem]--;
                    niveaux[0].inventaireJoueur1.itemPose = indexItem;
                    niveaux[0].inventaireJoueur1.posItemX = balleX1;
                    niveaux[0].inventaireJoueur1.posItemY = balleY1;
                    niveaux[0].inventaireJoueur1.sommetPile--;
                    printf("Joueur 1 : Item %s pose\n", niveaux[0].inventaireJoueur1.itemNames[indexItem]);
                } else if (niveaux[0].inventaireJoueur1.itemPose != -1) {
                    // Reprendre l'item
                    int indexItem = niveaux[0].inventaireJoueur1.itemPose;
                    niveaux[0].inventaireJoueur1.items[indexItem]++;
                    niveaux[0].inventaireJoueur1.sommetPile++;
                    niveaux[0].inventaireJoueur1.pileItemsPris[niveaux[0].inventaireJoueur1.sommetPile] = indexItem;
                    printf("Joueur 1 : Item %s repris\n", niveaux[0].inventaireJoueur1.itemNames[indexItem]);
                    niveaux[0].inventaireJoueur1.itemPose = -1;
                    niveaux[0].inventaireJoueur1.posItemX = -1;
                    niveaux[0].inventaireJoueur1.posItemY = -1;
                }
                key_c_pressed1 = 1;
            }
        }

        // Dessiner l'item posé pour le joueur 1
        if (niveaux[0].inventaireJoueur1.itemPose != -1) {
            // Dessiner l'image de l'item en utilisant la position stockée
            if (niveaux[0].inventaireJoueur1.itemPose == 5) {  // Vérifier le type d'item (ex: poisson cru)
                draw_sprite(screen, imagePoissonCru, niveaux[0].inventaireJoueur1.posItemX, niveaux[0].inventaireJoueur1.posItemY);
            } else if (niveaux[0].inventaireJoueur1.itemPose == 6) {  // Vérifier le type d'item (ex: poisson cuit)
                draw_sprite(screen, imagePoissonCuit, niveaux[0].inventaireJoueur1.posItemX, niveaux[0].inventaireJoueur1.posItemY);
            } else if (niveaux[0].inventaireJoueur1.itemPose == 0) {
                draw_sprite(screen, imageLegume, niveaux[0].inventaireJoueur1.posItemX, niveaux[0].inventaireJoueur1.posItemY);
            } else if (niveaux[0].inventaireJoueur1.itemPose == 1) {
                draw_sprite(screen, imageLegumeDecoupe, niveaux[0].inventaireJoueur1.posItemX, niveaux[0].inventaireJoueur1.posItemY);
            } else if (niveaux[0].inventaireJoueur1.itemPose == 7) {
                draw_sprite(screen, imageAssiette, niveaux[0].inventaireJoueur1.posItemX, niveaux[0].inventaireJoueur1.posItemY);
            } else if (niveaux[0].inventaireJoueur1.itemPose == 9) {
                draw_sprite(screen, imageCeviche, niveaux[0].inventaireJoueur1.posItemX, niveaux[0].inventaireJoueur1.posItemY);
            }
        }

        // Lorsque le joueur est dans une zone où il peut jeter un item
        if (balleX1 >= 521 && balleX1 <= 539 && balleY1 >= 106 && balleY1 <= 117) {
            if (key[KEY_V] && !key_v_pressed1) {
                jeterDernierItem(&niveaux[0].inventaireJoueur1);
                key_v_pressed1 = 1;  // Empêche la répétition jusqu'à ce que la touche soit relâchée
            }
        }

        // Affichage de la jauge de cuisson pour le Joueur 1
        if (niveaux[0].cuisson_commencee_j1) {
            double tempsEcoule = time(NULL) - niveaux[0].temps_debut_cuisson_j1;
            double pourcentage = tempsEcoule / 3.0; // Durée totale de cuisson de 3 secondes
            if (pourcentage > 1.0) pourcentage = 1.0; // S'assurer que le pourcentage ne dépasse pas 100%

            // Dessiner la jauge au-dessus de la tête du joueur 1
            dessiner_jauge(balleX1 - 20, balleY1 - 30, 40, 5, pourcentage);
        }

        // Affichage de la jauge de decoupe pour le Joueur 1
        if (niveaux[0].decoupe_commencee_j1) {
            double tempsEcoule = time(NULL) - niveaux[0].temps_debut_decoupe_j1;
            double pourcentage = tempsEcoule / 2.0; // Durée totale de decoupe de 2 secondes
            if (pourcentage > 1.0) pourcentage = 1.0; // S'assurer que le pourcentage ne dépasse pas 100%

            // Dessiner la jauge au-dessus de la tête du joueur 1
            dessiner_jauge(balleX1 - 20, balleY1 - 30, 40, 5, pourcentage);
        }

        if (!key[KEY_V]) {
            key_v_pressed1 = 0;
        }

        // Réinitialisation de l'état de la touche C lorsque celle-ci est relâchée
        if (!key[KEY_C]) {
            key_c_pressed1 = 0;
        }

        // Déplacement de la balle2
        int nouvellePosX2 = balleX2;
        int nouvellePosY2 = balleY2;
        if (key[KEY_UP]) nouvellePosY2--;
        if (key[KEY_DOWN]) nouvellePosY2++;
        if (key[KEY_LEFT]) nouvellePosX2--;
        if (key[KEY_RIGHT]) nouvellePosX2++;

        // Vérifier si les nouvelles coordonnées de la balle2 tombent dans une zone interdite
        int collision2 = 0;
        for (int i = 0; i < sizeof(zonesInterdites) / sizeof(zonesInterdites[0]); i++) {
            if (nouvellePosX2 >= zonesInterdites[i].x1 && nouvellePosX2 <= zonesInterdites[i].x2 &&
                nouvellePosY2 >= zonesInterdites[i].y1 && nouvellePosY2 <= zonesInterdites[i].y2) {
                collision2 = 1;
                break;
            }
        }

        // Vérifier si la nouvelle position de la balle2 entre en collision avec la balle1
        if (!collision2 && abs(nouvellePosX2 - balleX1) < TAILLE_BALLE && abs(nouvellePosY2 - balleY1) < TAILLE_BALLE) {
            collision2 = 1;
        }

        // Si aucune collision, déplacer la balle2
        if (!collision2) {
            balleX2 = nouvellePosX2;
            balleY2 = nouvellePosY2;
        }

        // Intéraction spécifique dans la zone pour récupérer une assiette
        if (balleX2 >= 490 && balleX2 <= 643 && balleY2 >= 279 && balleY2 <= 297) {
            if (key[KEY_L] && !key_l_pressed2) {
                ajouterItem(&niveaux[0].inventaireJoueur2, 7);
                printf("Joueur 2 : Assiette ajoutee a l'inventaire");
                key_l_pressed2 = 1;
            }
        }

        // Dessine l'image de l'assiette si le joueur a une assiette
        if (niveaux[0].inventaireJoueur2.items[7] > 0) {
            draw_sprite(screen, imageAssiette, balleX2 + 20, balleY2 - 20); // Ajuste la position relative au personnage
        }

        // Interaction spécifique dans la zone pour le poisson cru
        if (balleX2 >= 93 && balleX2 <= 110 && balleY2 >= 181 && balleY2 <= 210) {
            // Gère la prise du poisson cru lorsque la touche L est pressée et que la touche n'était pas déjà pressée
            if (key[KEY_L] && !key_l_pressed2) {
                ajouterItem(&niveaux[0].inventaireJoueur2, 5);
                printf("Joueur 2 : Ajoute a l'inventaire: %s\n", niveaux[0].inventaireJoueur2.itemNames[5]);
                key_l_pressed2 = 1;  // Empêche la répétition jusqu'à ce que la touche soit relâchée
            }
        }

        // Dessine l'image du poisson si le joueur a un poisson cru
        if (niveaux[0].inventaireJoueur2.items[5] > 0) {
            draw_sprite(screen, imagePoissonCru, balleX2 + 20, balleY2 - 20); // Ajuste la position relative au personnage
        }

        // Vérification et début de la cuisson pour le Joueur 2
        if ((balleX2 >= 307 && balleX2 <= 382 && balleY2 >= 146 && balleY2 <= 171) ||
            (balleX2 >= 303 && balleX2 <= 384 && balleY2 >= 247 && balleY2 <= 279)) {
            if (key[KEY_SEMICOLON] && niveaux[0].inventaireJoueur2.items[5] > 0 && !niveaux[0].cuisson_commencee_j2) {
                niveaux[0].temps_debut_cuisson_j2 = time(NULL);  // Enregistre le moment du début de la cuisson
                niveaux[0].cuisson_commencee_j2 = 1;
            }
        }

        // Vérifier si la cuisson est terminée
        if (niveaux[0].cuisson_commencee_j2 && (time(NULL) - niveaux[0].temps_debut_cuisson_j2) >= 3) {
            if (niveaux[0].inventaireJoueur2.items[5] > 0) {  // Assurez-vous qu'il y a des poissons crus
                niveaux[0].inventaireJoueur2.items[5]--;
                ajouterItem(&niveaux[0].inventaireJoueur2, 6);  // Index 6 pour 'poisson cuit'
                niveaux[0].cuisson_commencee_j2 = 0;
                printf("Joueur 2 : Poisson cuit ajoute a l'inventaire\n");
            } else {
                niveaux[0].cuisson_commencee_j2 = 0;  // Réinitialiser si aucune cuisson n'est possible
                printf("Joueur 2 : Aucun poisson cru a cuire\n");
            }
        }

        // Dessine l'image du poisson si le joueur 2 a un poisson cru
        if (niveaux[0].inventaireJoueur2.items[6] > 0) {  // Supposons que l'index 6 correspond au poisson cuit
            draw_sprite(screen, imagePoissonCuit, balleX2 + 20, balleY2 - 20); // Ajuste la position relative au personnage
        }

        // Intéraction spécifique dans la zone pour le legume
        if (balleX2 >= 102 && balleX2 <= 112 && balleY2 >= 140 && balleY2 <= 175) {
            // Gère la prise du legume lorsque la touche L est pressée et que la touche n'était pas déjà pressée
            if (key[KEY_L] && !key_l_pressed2) {
                ajouterItem(&niveaux[0].inventaireJoueur2, 0);
                printf("Joueur 2 : Ajoute a l'inventaire: %s\n", niveaux[0].inventaireJoueur2.itemNames[0]);
                key_l_pressed2 = 1;  // Empêche la répétition jusqu'à ce que la touche soit relâchée
            }
        }

        // Dessine l'image du legume si le joueur a un legume
        if (niveaux[0].inventaireJoueur2.items[0] > 0) {
            draw_sprite(screen, imageLegume, balleX2 + 20, balleY2 - 20); // Ajuste la position relative au personnage
        }

        // Intéraction pour la découpe du legume pour le joueur2
        if (balleX2 >= 220 && balleX2 <= 289 && balleY2 >= 199 && balleY2 <= 274) {
            if (key[KEY_L] && niveaux[0].inventaireJoueur2.items[0] > 0 && !niveaux[0].decoupe_commencee_j2) {
                niveaux[0].temps_debut_decoupe_j2 = time(NULL);
                niveaux[0].decoupe_commencee_j2 = 1;
            }
        }

        if (niveaux[0].decoupe_commencee_j2 && (time(NULL) - niveaux[0].temps_debut_decoupe_j2) >= 2) {
            if (niveaux[0].inventaireJoueur2.items[0] > 0) {
                niveaux[0].inventaireJoueur2.items[0]--;
                ajouterItem(&niveaux[0].inventaireJoueur2, 1);
                niveaux[0].decoupe_commencee_j2 = 1;
                printf("Joueur 2 : Tomate decoupee ajoute a l'inventaire\n");
            } else {
                niveaux[0].decoupe_commencee_j2 = 0;
                printf("Joueur 2 : Aucun legume a decouper\n");
            }
        }

        // Dessine l'image du legume decoupe si le joueur a un legume decoupe
        if (niveaux[0].inventaireJoueur2.items[1] > 0) {  // Supposons que l'index 1 correspond au legume decoupe
            draw_sprite(screen, imageLegumeDecoupe, balleX2 + 20, balleY2 - 20); // Ajuste la position relative au personnage
        }

        // Intéraction pour avoir un poisson a la sauce tomate
        if (niveaux[0].inventaireJoueur2.items[1] > 0 && niveaux[0].inventaireJoueur2.items[6] > 0) {
            niveaux[0].inventaireJoueur2.items[1]--;
            niveaux[0].inventaireJoueur2.items[6]--;
            ajouterItem(&niveaux[0].inventaireJoueur2, 8);
            printf("Joueur 2 : Poisson a la sauce tomate ajoute a l'inventaire\n");
        }

        // Dessine l'image du poisson cuit a la sauce tomate si le joueur 2 en a un
        if (niveaux[0].inventaireJoueur2.items[8] > 0) {
            draw_sprite(screen, imagePoissonTomate, balleX2 + 20, balleY2 - 20); // Ajuste la position relative au personnage
        }

        // Intéraction pour avoir un ceviche si le joueur 2 en a un
        if (niveaux[0].inventaireJoueur2.items[1] > 0 && niveaux[0].inventaireJoueur2.items[5] > 0) {
            niveaux[0].inventaireJoueur2.items[1]--;
            niveaux[0].inventaireJoueur2.items[5]--;
            ajouterItem(&niveaux[0].inventaireJoueur2, 9);
            printf("Joueur 2 : Ceviche a la tomate ajoutee a l'inventaire\n");
        }

        // Dessine l'image du ceviche si le joueur 2 en a un
        if (niveaux[0].inventaireJoueur2.items[9] > 0) {
            draw_sprite(screen, imageCeviche, balleX2 + 20, balleY2 - 20);
        }

        // Interaction pour poser ou reprendre un item pour le joueur 2
        if (balleX2 >= 222 && balleX2 <= 298 && balleY2 >= 155 && balleY2 <= 198) {
            if (key[KEY_L] && !key_l_pressed2) {
                if (niveaux[0].inventaireJoueur2.itemPose == -1 && niveaux[0].inventaireJoueur2.sommetPile >= 0) {
                    // Poser l'item
                    int indexItem2 = niveaux[0].inventaireJoueur2.pileItemsPris[niveaux[0].inventaireJoueur2.sommetPile];
                    niveaux[0].inventaireJoueur2.items[indexItem2]--;
                    niveaux[0].inventaireJoueur2.itemPose = indexItem2;
                    niveaux[0].inventaireJoueur2.posItemX = balleX2;
                    niveaux[0].inventaireJoueur2.posItemY = balleY2;
                    niveaux[0].inventaireJoueur2.sommetPile--;
                    printf("Joueur 2 : Item %s posé\n", niveaux[0].inventaireJoueur2.itemNames[indexItem2]);
                } else if (niveaux[0].inventaireJoueur2.itemPose != -1) {
                    // Reprendre l'item
                    int indexItem2 = niveaux[0].inventaireJoueur2.itemPose;
                    niveaux[0].inventaireJoueur2.items[indexItem2]++;
                    niveaux[0].inventaireJoueur2.sommetPile++;
                    niveaux[0].inventaireJoueur2.pileItemsPris[niveaux[0].inventaireJoueur2.sommetPile] = indexItem2;
                    printf("Joueur 2 : Item %s repris\n", niveaux[0].inventaireJoueur2.itemNames[indexItem2]);
                    niveaux[0].inventaireJoueur2.itemPose = -1;
                    niveaux[0].inventaireJoueur2.posItemX = -1;
                    niveaux[0].inventaireJoueur2.posItemY = -1;
                }
                key_l_pressed2 = 1;
            }
        }

        // Dessiner l'item posé pour le joueur 2
        if (niveaux[0].inventaireJoueur2.itemPose != -1) {
            // Dessiner l'image de l'item en utilisant la position stockée
            if (niveaux[0].inventaireJoueur2.itemPose == 5) {  // Vérifier le type d'item (ex: poisson cru)
                draw_sprite(screen, imagePoissonCru, niveaux[0].inventaireJoueur2.posItemX, niveaux[0].inventaireJoueur2.posItemY);
            } else if (niveaux[0].inventaireJoueur2.itemPose == 6) {  // Vérifier le type d'item (ex: poisson cuit)
                draw_sprite(screen, imagePoissonCuit, niveaux[0].inventaireJoueur2.posItemX, niveaux[0].inventaireJoueur2.posItemY);
            } else if (niveaux[0].inventaireJoueur2.itemPose == 0) {
                draw_sprite(screen, imageLegume, niveaux[0].inventaireJoueur2.posItemX, niveaux[0].inventaireJoueur2.posItemY);
            } else if (niveaux[0].inventaireJoueur2.itemPose == 1) {
                draw_sprite(screen, imageLegumeDecoupe, niveaux[0].inventaireJoueur2.posItemX, niveaux[0].inventaireJoueur2.posItemY);
            } else if (niveaux[0].inventaireJoueur2.itemPose == 7) {
                draw_sprite(screen, imageAssiette, niveaux[0].inventaireJoueur2.posItemX, niveaux[0].inventaireJoueur2.posItemY);
            } else if (niveaux[0].inventaireJoueur2.itemPose == 8) {
                draw_sprite(screen, imagePoissonTomate, niveaux[0].inventaireJoueur2.posItemX, niveaux[0].inventaireJoueur2.posItemY);
            } else if (niveaux[0].inventaireJoueur2.itemPose == 9) {
                draw_sprite(screen, imageCeviche, niveaux[0].inventaireJoueur2.posItemX, niveaux[0].inventaireJoueur2.posItemY);
            }
        }

        // Intéraction pour livrer les plats pour le joueur 2
        if (balleX2 >= 259 && balleX2 <= 416 && balleY2 >= 388 && balleY2 <= 440) {
            if (key[KEY_SEMICOLON] && !key_m_pressed2) {
                if (niveaux[0].inventaireJoueur2.items[8] > 0) {
                    verifierLivraison(&niveaux[0].inventaireJoueur2, 8, &niveaux[0].scoreJoueur2, &niveaux[0].nombreLivraisonsJoueur2, tempsRestant);  // Vérifie la livraison pour le poisson à la sauce tomate
                }
                if (niveaux[0].inventaireJoueur2.items[9] > 0) {
                    verifierLivraison(&niveaux[0].inventaireJoueur2, 9, &niveaux[0].scoreJoueur2, &niveaux[0].nombreLivraisonsJoueur2, tempsRestant);  // Vérifie la livraison pour le ceviche
                }
                key_m_pressed2 = 1;
            }
        }

        // Réinitialisation de l'état de la touche L lorsque celle-ci est relâchée
        if (!key[KEY_L]) {
            key_l_pressed2 = 0;
        }

        // Réinitialisation de l'état de la touche SEMICOLON lorsque celle-ci est relâchée
        if (!key[KEY_SEMICOLON]) {
            key_m_pressed2 = 0;
        }

        // Affichage de la jauge de cuisson pour le Joueur 2
        if (niveaux[0].cuisson_commencee_j2) {
            double tempsEcoule = time(NULL) - niveaux[0].temps_debut_cuisson_j2;
            double pourcentage = tempsEcoule / 3.0; // Durée totale de cuisson de 3 secondes
            if (pourcentage > 1.0) pourcentage = 1.0; // S'assurer que le pourcentage ne dépasse pas 100%

            // Dessiner la jauge au-dessus de la tête du joueur 2
            dessiner_jauge(balleX2 - 20, balleY2 - 30, 40, 5, pourcentage);
        }

        // Affichage de la jauge de decoupe pour le Joueur 2
        if (niveaux[0].decoupe_commencee_j2) {
            double tempsEcoule = time(NULL) - niveaux[0].temps_debut_decoupe_j2;
            double pourcentage = tempsEcoule / 2.0; // Durée totale de decoupe de 2 secondes
            if (pourcentage > 1.0) pourcentage = 1.0; // S'assurer que le pourcentage ne dépasse pas 100%

            // Dessiner la jauge au-dessus de la tête du joueur 2
            dessiner_jauge(balleX2 - 20, balleY2 - 30, 40, 5, pourcentage);
        }

        // Lorsque le joueur 2 est dans une zone où il peut jeter un item
        if (balleX2 >= 521 && balleX2 <= 539 && balleY2 >= 106 && balleY2 <= 117) {
            if (key[KEY_L] && !key_l_pressed2) {
                jeterDernierItem(&niveaux[0].inventaireJoueur2);
                key_l_pressed2 = 1;
            }
        }

        // Si les balles entrent en collision, réinitialiser leurs positions précédentes
        if (collision1 || collision2) {
            balleX1 = ancienX1;
            balleY1 = ancienY1;
            balleX2 = ancienX2;
            balleY2 = ancienY2;
        }

        // Vérifier que les balles restent à l'intérieur de l'écran et dans les bordures de la carte
        if (balleX1 < 0) balleX1 = 0;
        if (balleX1 >= LARGEUR_ECRAN) balleX1 = LARGEUR_ECRAN - 1;
        if (balleY1 < 0) balleY1 = 0;
        if (balleY1 >= HAUTEUR_ECRAN) balleY1 = HAUTEUR_ECRAN - 1;

        if (balleX2 < 0) balleX2 = 0;
        if (balleX2 >= LARGEUR_ECRAN) balleX2 = LARGEUR_ECRAN - 1;
        if (balleY2 < 0) balleY2 = 0;
        if (balleY2 >= HAUTEUR_ECRAN) balleY2 = HAUTEUR_ECRAN - 1;

        // Vérifier si la balle1 est dans une des zones de messages
        for (int i = 0; i < sizeof(zonesMessages) / sizeof(zonesMessages[0]); i++) {
            if (balleX1 >= zonesMessages[i].zone.x1 && balleX1 <= zonesMessages[i].zone.x2 &&
                balleY1 >= zonesMessages[i].zone.y1 && balleY1 <= zonesMessages[i].zone.y2) {
                textout_centre_ex(screen, font, zonesMessages[i].message, balleX1, balleY1 + 20, makecol(255, 255, 255), -1);
            }
        }

        // Vérifier si la balle2 est dans une des zones de messages
        for (int i = 0; i < sizeof(zonesMessages) / sizeof(zonesMessages[0]); i++) {
            if (balleX2 >= zonesMessages[i].zone.x1 && balleX2 <= zonesMessages[i].zone.x2 &&
                balleY2 >= zonesMessages[i].zone.y1 && balleY2 <= zonesMessages[i].zone.y2) {
                textout_centre_ex(screen, font, zonesMessages[i].message, balleX2, balleY2 + 20, makecol(255, 255, 255), -1);
            }
        }

        // Calcul du temps restant
        tempsRestant = tempsTotal - ((int)time(NULL) - tempsDebut);
        if (tempsRestant < 0) tempsRestant = 0;

        // Affichage du minuteur en haut à droite de l'écran
        afficher_minuteur(tempsRestant);

        // Afficher les commandes en bas à gauche de l'écran
        afficherCommandes();

        if (key[KEY_EQUALS]) {
            stop_sample(musique_fond);
        }

        if (tempsRestant <= 0) {
            clear_bitmap(screen);
            textprintf_right_ex(screen, font, LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2, makecol(255, 255, 255), -1, "Vous avez perdu");
            rest(3000);
            clear_bitmap(screen);
            textprintf_right_ex(screen, font, LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2, makecol(255, 255, 255), -1, "Taper 1 pour recommencer ou taper 0 pour quitter");
            readkey();
            clear_bitmap(screen);
            stop_sample(musique_fond);
            int choix;
            while (1) {
                if (keypressed()) { // Vérifier si une touche est enfoncée
                    choix = readkey() & 0xFF; // Lire la touche enfoncée
                    if (choix == '1' || choix == '0') break; // Sortir de la boucle si l'utilisateur appuie sur 1 ou 0
                }
            }

            // Si l'utilisateur choisit de recommencer, appeler jouerPartie()
            if (choix == '1') {
                jouerPartie();
            } else if (choix == '0') {
                afficherMenu(); // Si l'utilisateur choisit de revenir au menu, afficher le menu
            }
        }

        // Rafraîchit l'écran
        vsync();
    }

    destroy_bitmap(imagePoissonCru);
    destroy_bitmap(imagePoissonCuit);
    destroy_bitmap(imageCeviche);
    destroy_bitmap(imageLegumeDecoupe);
    destroy_bitmap(imageLegume);
    destroy_bitmap(grille1);
    destroy_bitmap(imageAssiette);
    destroy_bitmap(imagePoissonTomate);

    // Arrête la musique de fond
    stop_sample(musique_fond);

    // Nettoyage
    destroy_sample(musique_fond);
}
 */
