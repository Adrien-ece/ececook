//
// Created by FA706 on 05/04/2024.
//
#include "game.h"
#include "regles.h"
#include "generation.h"
#include <stdio.h>

void afficherRegle() {
    // Charger la musique de fond
    SAMPLE* musique_fond = charger_musique("background_music.wav");
    if (!musique_fond) {
        return;
    }

    play_sample(musique_fond, 255, 128, 1000, 1);
    // Charger l'image de fond pour les règles du jeu
    BITMAP* fond_menu = charger_image("../regles.bmp");
    if (!fond_menu) {
        return;
    }

    // Afficher le fond d'écran
    blit(fond_menu, screen, 0, 0, 0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN);

    // Libérer la mémoire utilisée par l'image de fond
    destroy_bitmap(fond_menu);

    show_mouse(screen);

    int choix;
    while (1) {
        if (keypressed()) { // Vérifier si une touche est enfoncée
            choix = readkey() & 0xFF; // Lire la touche enfoncée
            if (choix == '1' || choix == '0') break; // Sortir de la boucle si l'utilisateur appuie sur 1 ou 0
        }
    }

    // Si l'utilisateur choisit de recommencer, appeler afficherRegle()
    if (choix == '1') {
        afficherRegle();
    } else if (choix == '0') {
        stop_sample(musique_fond);
        afficherMenu(); // Si l'utilisateur choisit de revenir au menu, afficher le menu
    }
}
