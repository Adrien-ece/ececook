//
// Created by FA706 on 05/04/2024.
//

#include "generation.h"
#include "game.h"
// Fonction pour charger la musique de fond à partir d'un fichier
SAMPLE* charger_musique(const char* chemin) {
    SAMPLE* musique = load_sample(chemin);
    if (!musique) {
        allegro_message("Impossible de charger la musique de fond !");
        return NULL;
    }
    return musique;
}

// Fonction pour charger une image à partir d'un fichier
BITMAP* charger_image(const char* chemin) {
    BITMAP* image = load_bitmap(chemin, NULL);
    if (!image) {
        allegro_message("Impossible de charger l'image !");
        return NULL;
    }
    return image;
}
