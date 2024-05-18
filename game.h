#ifndef GAME_H
#define GAME_H

#include <allegro.h>

#define LARGEUR_ECRAN 680
#define HAUTEUR_ECRAN 440
#define TAILLE_BALLE 20
#define MAX_ITEMS 10

// Déclaration des structures
typedef struct {
    int x1, y1, x2, y2;
} Rectangle;

typedef struct {
    Rectangle zone;
    char message[100];
} ZoneMessage;

typedef struct {
    int items[MAX_ITEMS];  // Quantité de chaque item
    char itemNames[MAX_ITEMS][50];  // Noms des items
    int pileItemsPris[MAX_ITEMS];
    int sommetPile;
    int itemPose;  // Ajout pour stocker l'item posé
    int posItemX;  // Ajout pour stocker la position X de l'item posé
    int posItemY;  // Ajout pour stocker la position Y de l'item posé

} Inventaire;

typedef struct {
    int itemIndex;  // Index de l'item dans l'inventaire
    BITMAP *image;  // Image représentant le plat
} Commande;

// Déclarations de variables externes
extern char pseudoJoueur1[50];
extern char pseudoJoueur2[50];
extern Rectangle zonesInterdites[];
extern ZoneMessage zonesMessages[];
extern Inventaire inventaireJoueur1;
extern Inventaire inventaireJoueur2;
extern Commande commandes[2];
extern int scoreJoueur1;
extern int scoreJoueur2;

// Déclarations de fonctions
void afficher_minuteur(int tempsRestant);
void saisirPseudo(char pseudo[], int joueur);
void jouerPartie();
void afficherMenu();
void afficherRegle();
void dessiner_jauge(int x, int y, int largeur, int hauteur, double pourcentage);
void initialiserCommandes();
void afficherCommandes();
void verifierLivraison(Inventaire *inventaire, int itemIndex, int *score, int *nombreLivraisons, int tempsRestant);
void afficherScores();

#endif /* GAME_H */
