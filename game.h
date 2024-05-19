#ifndef GAME_H
#define GAME_H

#include "generation.h"
#include <allegro.h>

#define MAX_NIVEAUX 5
#define LARGEUR_ECRAN 680
#define HAUTEUR_ECRAN 440
#define MAX_ITEMS 10
#define TAILLE_BALLE 20

// Structures
typedef struct {
    int x1, y1, x2, y2;
} Rectangle;

typedef struct {
    Rectangle zone;
    char message[50];
} ZoneMessage;

typedef struct {
    int items[MAX_ITEMS];
    char itemNames[MAX_ITEMS][50];
    int pileItemsPris[MAX_ITEMS];
    int sommetPile;
    int itemPose;
    int posItemX;
    int posItemY;
} Inventaire;

typedef struct {
    int itemIndex;
    BITMAP *image;
} Commande;

typedef struct {
    Inventaire inventaireJoueur1;
    Inventaire inventaireJoueur2;
    Commande commandes[2];
    int scoreJoueur1;
    int scoreJoueur2;
    int nombreLivraisonsJoueur1;
    int nombreLivraisonsJoueur2;
    int cuisson_commencee_j1;
    int cuisson_commencee_j2;
    int decoupe_commencee_j1;
    int decoupe_commencee_j2;
    time_t temps_debut_cuisson_j1;
    time_t temps_debut_cuisson_j2;
    time_t temps_debut_decoupe_j1;
    time_t temps_debut_decoupe_j2;
} Niveau;

// Variables globales
extern Niveau niveaux[MAX_NIVEAUX];
extern char pseudoJoueur1[50];
extern char pseudoJoueur2[50];
extern Rectangle zonesInterdites[];
extern ZoneMessage zonesMessages[];
extern int niveauActuel;

// Prototypes de fonctions
void afficher_minuteur(int tempsRestant);
void reinitialiserCommandes(int niveau);
void initialiserInventaire(Inventaire *inventaire);
void ajouterItem(Inventaire *inventaire, int indexItem);
void jeterDernierItem(Inventaire *inventaire);
void dessiner_jauge(int x, int y, int largeur, int hauteur, double pourcentage);
void verifierLivraison(Inventaire *inventaire, int itemIndex, int *score, int *nombreLivraisons, int tempsRestant);
void initialiserCommandes();
void afficherCommandes();
void afficherScores();
void chargerItems(Inventaire *inventaire);
void saisirPseudo(char pseudo[], int joueur);
void jouerPartie();
void afficherMenu();
BITMAP* charger_image(const char* filename);
SAMPLE* charger_musique(const char* filename);
void jouerNiveau2();

#endif // GAME_H
