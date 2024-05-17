//
// Created by FA706 on 05/04/2024.
//
#include "game.h"
#include "regles.h"
#include "generation.h"
#include "menu.h"

void afficherMenu() {
    // Charger l'image de fond pour le menu
    BITMAP* fond_menu = charger_image("../menu.bmp");
    if (!fond_menu) {
        return;
    }
    // Charger la musique de fond
    SAMPLE* musique_fond = charger_musique("background_music.wav");
    if (!musique_fond) {
        return;
    }
    stop_sample(musique_fond);
    play_sample(musique_fond, 255, 128, 1000, 1);
    // Afficher le fond d'écran
    blit(fond_menu, screen, 0, 0, 0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN);

    // Libérer la mémoire utilisée par l'image de fond
    destroy_bitmap(fond_menu);

    // Affichage de la souris dans le menu
    show_mouse(screen);

    // Boucle principale pour le menu
    while (!key[KEY_ESC]) {
        int mouseX = mouse_x;
        int mouseY = mouse_y;

        if (mouse_b & 1) {
            if (mouseX > 11 && mouseX < 195 && mouseY > 232 && mouseY < 289) {
                // Si l'utilisateur clique sur "Nouvelle Partie"
                clear_bitmap(screen); // Efface l'écran
                textout_centre_ex(screen, font, "Jeu en cours...", LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2,
                                  makecol(255, 255, 255), -1);
                rest(2000);// Attend 2 secondes
                stop_sample(musique_fond);
                jouerPartie();
                break; // Sort de la boucle du menu
            }
            if (mouseX > 492 && mouseX < 672 && mouseY < 281 && mouseY > 224) {
                clear_bitmap(screen);
                textout_centre_ex(screen, font, "Affichage des regles du jeu...", LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2,
                                  makecol(255, 0, 0), -1);
                rest(1000);
                stop_sample(musique_fond);
                afficherRegle();
            }
            if (mouseX > 245 && mouseX < 431 && mouseY <425 && mouseY > 369){
                allegro_exit();
            }
        }
    }

    // Masquer la souris avant de commencer le jeu
    show_mouse(NULL);
}
