#ifndef GESTION_LIVRE_BIBLIOTHÈQUE_H
#define GESTION_LIVRE_BIBLIOTHÈQUE_H
#ifndef BIBLIOTHEQUE_H
#define BIBLIOTHEQUE_H

#include <stdbool.h>
#include <stddef.h>

/* Tailles maximales (incluent le '\0') */
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

/* Codes de statut / erreur pour les opérations */
typedef enum {
    STATUT_OK = 0,
    STATUT_DOUBLON_ISBN,
    STATUT_NON_TROUVE,
    STATUT_ENTREE_INVALIDE,
    STATUT_PLEIN,
    STATUT_MEMOIRE,
    STATUT_INTERNE
    } StatutLivre;

/* Modèle principal d'un livre */
typedef struct {
    char isbn[ISBN_LEN]; /* Unique dans la bibliothèque */
    char titre[TITRE_LEN];
    char auteur[AUTEUR_LEN];
    int annee; /* ex: 1450..2100 */
    char categorie[CATEGORIE_LEN]; /* ex: "Roman", "Informatique" */
    Disponibilite dispo;
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

void freeBibliotheque(Bibliotheque *b);
#endif /* BIBLIOTHEQUE_H */

#endif //GESTION_LIVRE_BIBLIOTHÈQUE_H
