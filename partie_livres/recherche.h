//
// Created by marie on 03/11/2025.
//

#ifndef RECHERCHE_H
#define RECHERCHE_H



#include <stdbool.h>
#include <stddef.h>
/* Tailles maximales (incluent le '\0') */
#define ISBN_LEN 18 /* ex: "978-2-07-036822-8" */
#define TITRE_LEN 128
#define AUTEUR_LEN 96
#define CATEGORIE_LEN 48
#define ISBN_LEN 18 /* ex: "978-2-07-036822-8" */
#define TITRE_LEN 128
#define AUTEUR_LEN 96
#define CATEGORIE_LEN 48
/* Disponibilité / état d'un livre */
typedef enum {
    LIVRE_DISPONIBLE = 0,
    LIVRE_EMPRUNTE,
    LIVRE_RESERVE,
    LIVRE_PERDU,
    LIVRE_HORS_SERVICE
    } Disponibilite;

typedef enum {
    STATUT_OK = 0,
    STATUT_DOUBLON_ISBN,
    STATUT_NON_TROUVE,
    STATUT_ENTREE_INVALIDE,
    STATUT_PLEIN,
    STATUT_MEMOIRE,
    STATUT_INTERNE,
    // 🆕 pour les emprunts
    STATUT_LIVRE_INEXISTANT,
    STATUT_PLUS_DISPONIBLE
    } StatutLivre;

/* Modèle principal d'un livre */
typedef struct {
    char isbn[ISBN_LEN]; /* Unique dans la bibliothèque */
    char titre[TITRE_LEN];
    char auteur[AUTEUR_LEN];
    int annee; /* ex: 1450..2100 */
    char categorie[CATEGORIE_LEN]; /* ex: "Roman", "Informatique" */
    Disponibilite dispo; /* état global : dispo, perdu, HS, etc. */

    // 🆕 gestion des exemplaires
    int nb_exemplaires_total; /* nb d'exemplaires possédés */
    int nb_exemplaires_disponibles;/* nb d'exemplaires actuellement dispo */
    int nb_Emprunts;

} Livre;


/* Masques de mise à jour partielle pour UpdateLivre.mask */
enum {
    UPD_TITRE = 1u << 0,
    UPD_AUTEUR = 1u << 1,
    UPD_ANNEE = 1u << 2,
    UPD_CATEGORIE = 1u << 3,
    UPD_DISPO = 1u << 4
    };

/* Conteneur de mise à jour partielle :
- 'mask' indique quels champs appliquer. */
typedef struct {
    unsigned int mask; /* Combinaison de UPD_* */
    char new_titre[TITRE_LEN];
    char new_auteur[AUTEUR_LEN];
    int new_annee;
    char new_categorie[CATEGORIE_LEN];
    Disponibilite new_dispo;
} UpdateLivre;

/* Conteneur bibliothèque (tableau dynamique + compteurs) */
typedef struct {
    Livre *livres; /* tableau alloué dynamiquement */
    size_t nb; /* nombre d'éléments utilisés */
    size_t capacite; /* taille allouée (en Livres) */
} Bibliotheque;

StatutLivre ajouterLivre(Bibliotheque *b, const Livre *livre);

StatutLivre supprimerLivre(Bibliotheque *b, const char *isbn);

StatutLivre modifierLivre(Bibliotheque *b, const char *isbn, const UpdateLivre *u);

StatutLivre initBibliotheque(Bibliotheque *b);

StatutLivre sauvegarderBibliotheque(const Bibliotheque *b, const char *filename);

StatutLivre chargerBibliotheque(Bibliotheque *b, const char *filename);

int getNbExemplairesDisponibles(const Bibliotheque *b, const char *isbn);
StatutLivre emprunterExemplaire(Bibliotheque *b, const char *isbn);
StatutLivre retournerExemplaire(Bibliotheque *b, const char *isbn);


void afficherEnteteTable();
void afficherTousLesLivresTable(const Bibliotheque *b);
StatutLivre sauvegarderBibliothequeTable(const Bibliotheque *b, const char *filename);


void menu_livres(Bibliotheque* b,int role);
StatutLivre setDisponibiliteLivre(Bibliotheque *b, const char *isbn, Disponibilite d);


void freeBibliotheque(Bibliotheque *b);


void rechercherLivreParTitre(const Bibliotheque *b, const char *titre);
void rechercherLivreParAuteur(const Bibliotheque *b, const char *auteur);
void rechercherLivreParISBN(const Bibliotheque *b, const char *isbn);
void rechercherLivreParCategorie(const Bibliotheque *b, const char *categorie);
void afficherLivresParTitre(const Bibliotheque *b);
void afficherLivresParAnnee(const Bibliotheque *b);
void afficherDisponibiliteLivre(const Bibliotheque *b, const char *isbn);
StatutLivre setDisponibiliteLivre(Bibliotheque *b, const char *isbn, Disponibilite d);
void afficherLivresDuneAnnee(const Bibliotheque *b, int annee);

StatutLivre ajouterLivre(Bibliotheque *b, const Livre *livre);

StatutLivre supprimerLivre(Bibliotheque *b, const char *isbn);

StatutLivre modifierLivre(Bibliotheque *b, const char *isbn, const UpdateLivre *u);

StatutLivre initBibliotheque(Bibliotheque *b);


StatutLivre chargerBibliotheque(Bibliotheque *b, const char *filename);

int getNbExemplairesDisponibles(const Bibliotheque *b, const char *isbn);
StatutLivre emprunterExemplaire(Bibliotheque *b, const char *isbn);
StatutLivre retournerExemplaire(Bibliotheque *b, const char *isbn);


void afficherEnteteTable();
void afficherTousLesLivresTable(const Bibliotheque *b);
StatutLivre sauvegarderBibliothequeTable(const Bibliotheque *b, const char *filename);


void menu_livres(Bibliotheque* b, int role);
StatutLivre setDisponibiliteLivre(Bibliotheque *b, const char *isbn, Disponibilite d);


void freeBibliotheque(Bibliotheque *b);
#endif //RECHERCHE_H
