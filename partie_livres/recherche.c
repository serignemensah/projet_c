//
// Created by marie on 03/11/2025.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "recherche.h"

void rechercherLivreParTitre(const Bibliotheque *b, const char *titre) {
    int trouvé = 0;
    for (size_t i = 0; i < b->nb; i++) {
        if (strstr(b->livres[i].titre, titre)) {
            printf(" %s | %s | %s | %d\n",
                   b->livres[i].titre,
                   b->livres[i].auteur,
                   b->livres[i].isbn,
                   b->livres[i].annee);
            trouvé = 1;
        }
    }
    if (!trouvé) printf(" Aucun livre avec le titre contenant '%s'\n", titre);
}

void rechercherLivreParAuteur(const Bibliotheque *b, const char *auteur) {
    int trouvé = 0;
    for (size_t i = 0; i < b->nb; i++) {
        if (strstr(b->livres[i].auteur, auteur)) {
            printf("👤 %s | %s | %s | %d\n",
                   b->livres[i].auteur,
                   b->livres[i].titre,
                   b->livres[i].isbn,
                   b->livres[i].annee);
            trouvé = 1;
        }
    }
    if (!trouvé) printf(" Aucun livre d'un auteur contenant '%s'\n", auteur);
}

void rechercherLivreParISBN(const Bibliotheque *b, const char *isbn) {
    for (size_t i = 0; i < b->nb; i++) {
        if (strcmp(b->livres[i].isbn, isbn) == 0) {
            printf(" Livre trouvé : %s | %s | %d | %s\n",
                   b->livres[i].titre,
                   b->livres[i].auteur,
                   b->livres[i].annee,
                   b->livres[i].categorie);


            return;
        }
    }
    printf(" Aucun livre avec l'ISBN '%s'\n", isbn);
}

void rechercherLivreParCategorie(const Bibliotheque *b, const char *categorie) {
    int trouvé = 0;
    for (size_t i = 0; i < b->nb; i++) {
        if (strstr(b->livres[i].categorie, categorie)) {
            printf(" %s | %s | %s | %d\n",
                   b->livres[i].categorie,
                   b->livres[i].titre,
                   b->livres[i].auteur,
                   b->livres[i].annee);
            trouvé = 1;
        }
    }
    if (!trouvé) printf(" Aucun livre dans la catégorie contenant '%s'\n", categorie);
}

static int cmpTitre(const void *a, const void *b) {
    const Livre *l1 = (const Livre*)a;
    const Livre *l2 = (const Livre*)b;
    return strcmp(l1->titre, l2->titre);
}

// Compare années (plus récent -> plus ancien)
static int cmpAnnee(const void *a, const void *b) {
    const Livre *l1 = (const Livre*)a;
    const Livre *l2 = (const Livre*)b;
    return l2->annee - l1->annee;
    // pour ancien -> récent, inverse (l1->annee - l2->annee)
}

/* ---- Affichage générique ---- */
static void afficherLivre(const Livre *l) {
    printf("%s | %s | %s | %d | %s | %d\n",
        l->isbn, l->titre, l->auteur, l->annee, l->categorie, l->dispo);
}

/* ---- AFFICHAGE TRIÉ PAR TITRE ---- */
void afficherLivresParTitre(const Bibliotheque *b) {
    if (b->nb == 0) {
        printf(" Bibliothèque vide.\n");
        return;
    }

    Livre *tmp = malloc(b->nb * sizeof(Livre));
    if (!tmp) {
        printf(" Erreur mémoire\n");
        return;
    }

    memcpy(tmp, b->livres, b->nb * sizeof(Livre));

    qsort(tmp, b->nb, sizeof(Livre), cmpTitre);

    printf("\n Livres triés par Titre (A → Z) :\n");
    for (size_t i = 0; i < b->nb; i++) {
        afficherLivre(&tmp[i]);
    }

    free(tmp);
}

/* ---- AFFICHAGE TRIÉ PAR ANNÉE ---- */
void afficherLivresParAnnee(const Bibliotheque *b) {
    if (b->nb == 0) {
        printf("📂 Bibliothèque vide.\n");
        return;
    }

    Livre *tmp = malloc(b->nb * sizeof(Livre));
    if (!tmp) {
        printf("❌ Erreur mémoire\n");
        return;
    }

    memcpy(tmp, b->livres, b->nb * sizeof(Livre));

    qsort(tmp, b->nb, sizeof(Livre), cmpAnnee);

    printf("\n Livres triés par Année (plus récents → plus anciens) :\n");
    for (size_t i = 0; i < b->nb; i++) {
        afficherLivre(&tmp[i]);
    }

    free(tmp);
}

/* Convert enum disponibilité en texte lisible */
static const char* dispoToString(Disponibilite d) {
    switch(d) {
    case LIVRE_DISPONIBLE: return "Disponible ";
    case LIVRE_EMPRUNTE: return "Emprunté ";
    case LIVRE_RESERVE: return "Réservé ";
    case LIVRE_PERDU: return "Perdu ";
    case LIVRE_HORS_SERVICE: return "Hors service ";
    default: return "État inconnu";
    }
}

void afficherDisponibiliteLivre(const Bibliotheque *b, const char *isbn) {
    for (size_t i = 0; i < b->nb; i++) {
        if (strcmp(b->livres[i].isbn, isbn) == 0) {
            printf(" Livre : %s\n", b->livres[i].titre);
            printf("   ISBN : %s\n", b->livres[i].isbn);
            printf("   Statut : %s\n", dispoToString(b->livres[i].dispo));
            return;
        }
    }
    printf(" Aucun livre avec l'ISBN '%s'\n", isbn);
}

StatutLivre setDisponibiliteLivre(Bibliotheque *b, const char *isbn, Disponibilite d) {
    for (size_t i = 0; i < b->nb; i++) {
        if (strcmp(b->livres[i].isbn, isbn) == 0) {
            b->livres[i].dispo = d;
            return STATUT_OK;
        }
    }
    return STATUT_NON_TROUVE;
}

void afficherLivresDuneAnnee(const Bibliotheque *b, int annee) {
    if (b == NULL) {
        printf("Erreur: bibliotheque NULL\n");
        return;
    }

    if (b->nb == 0) {
        printf(" Bibliothèque vide.\n");
        return;
    }

    if (b->livres == NULL) {
        printf("Erreur: tableau de livres non initialisé.\n");
        return;
    }

    int trouve = 0;
    printf("\n Livres publiés en %d :\n", annee);
    for (size_t i = 0; i < b->nb; ++i) {
        /* sécurité : tu peux vérifier ranges si tu as un champ capacite */
        /* afficherLivre est ta fonction d'affichage existante */
        if (b->livres[i].annee == annee) {
            afficherLivre(&b->livres[i]);
            trouve = 1;
        }
    }

    if (!trouve) {
        printf(" Aucun livre publié en %d\n", annee);
    }
}