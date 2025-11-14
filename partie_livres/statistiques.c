//
// Created by marie on 03/11/2025.
//

#include "statistiques.h"
#include <stdio.h>
#include"bibliothèque.h"
#include <string.h>
#include <time.h>
int compterTotalLivres(const Bibliotheque *b) {
    return b->nb;
}

int compterLivresParStatut(const Bibliotheque *b, Disponibilite d) {
    int count = 0;
    for (size_t i = 0; i < b->nb; i++) {
        if (b->livres[i].dispo == d)
            count++;
    }
    return count;
}

int compterLivresDisponibles(const Bibliotheque *b) {
    return compterLivresParStatut(b, LIVRE_DISPONIBLE);
}

Livre* trouverLivrePlusAncien(const Bibliotheque *b) {
    if (b->nb == 0) return NULL;

    size_t idx = 0;
    for (size_t i = 1; i < b->nb; i++) {
        if (b->livres[i].annee < b->livres[idx].annee)
            idx = i;
    }
    return &b->livres[idx];
}

Livre* trouverLivrePlusRecent(const Bibliotheque *b) {
    if (b->nb == 0) return NULL;

    size_t idx = 0;
    for (size_t i = 1; i < b->nb; i++) {
        if (b->livres[i].annee > b->livres[idx].annee)
            idx = i;
    }
    return &b->livres[idx];
}

void afficherStatistiquesBibliotheque(const Bibliotheque *b) {
    printf("\n Statistiques Bibliothèque \n");

    printf(" Total livres              : %d\n", compterTotalLivres(b));
    printf(" Disponibles               : %d\n", compterLivresParStatut(b, LIVRE_DISPONIBLE));
    printf(" Empruntés                 : %d\n", compterLivresParStatut(b, LIVRE_EMPRUNTE));
    printf(" Réservés                  : %d\n", compterLivresParStatut(b, LIVRE_RESERVE));
    printf(" Perdus                    : %d\n", compterLivresParStatut(b, LIVRE_PERDU));
    printf(" Hors service              : %d\n", compterLivresParStatut(b, LIVRE_HORS_SERVICE));

    Livre *old = trouverLivrePlusAncien(b);
    Livre *new = trouverLivrePlusRecent(b);

    if (old)
        printf(" Livre le plus ancien      : %s (%d)\n", old->titre, old->annee);
    if (new)
        printf(" Livre le plus récent      : %s (%d)\n", new->titre, new->annee);
    afficherStatsParCategorie(b);
}
int compterLivresParCategorie(const Bibliotheque *b, const char *categorie) {
    int count = 0;
    for (size_t i = 0; i < b->nb; i++) {
        if (strcmp(b->livres[i].categorie, categorie) == 0)
            count++;
    }
    return count;
}

void afficherStatsParCategorie(const Bibliotheque *b) {
    if (b->nb == 0) {
        printf("📂 Aucune catégorie (bibliothèque vide)\n");
        return;
    }

    printf("\n📂 Livres par catégorie :\n");

    // On garde une liste locale des catégories déjà affichées
    char categories[100][CATEGORIE_LEN];
    int nbCat = 0;

    for (size_t i = 0; i < b->nb; i++) {

        // Vérifier si la catégorie a déjà été comptée
        int dejaVu = 0;
        for (int j = 0; j < nbCat; j++) {
            if (strcmp(categories[j], b->livres[i].categorie) == 0) {
                dejaVu = 1;
                break;
            }
        }

        if (!dejaVu) {
            // Nouvelle catégorie
            strcpy(categories[nbCat], b->livres[i].categorie);
            nbCat++;

            int count = compterLivresParCategorie(b, b->livres[i].categorie);
            printf("   %-15s : %d\n", b->livres[i].categorie, count);
        }
    }
}
StatutLivre genererRapportStatistiques(const Bibliotheque *b, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return STATUT_INTERNE;

    // Date actuelle
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    fprintf(f, " Rapport Statistiques Bibliothèque\n");
    fprintf(f, "----------------------------------\n\n");

    fprintf(f, " Total livres              : %d\n", compterTotalLivres(b));
    fprintf(f, " Disponibles               : %d\n", compterLivresParStatut(b, LIVRE_DISPONIBLE));
    fprintf(f, " Empruntés                 : %d\n", compterLivresParStatut(b, LIVRE_EMPRUNTE));
    fprintf(f, " Réservés                  : %d\n", compterLivresParStatut(b, LIVRE_RESERVE));
    fprintf(f, " Perdus                    : %d\n", compterLivresParStatut(b, LIVRE_PERDU));
    fprintf(f, " Hors service              : %d\n", compterLivresParStatut(b, LIVRE_HORS_SERVICE));

    Livre *old = trouverLivrePlusAncien(b);
    Livre *new = trouverLivrePlusRecent(b);

    if (old)
        fprintf(f, "\n Livre le plus ancien      : %s (%d)\n", old->titre, old->annee);
    if (new)
        fprintf(f, " Livre le plus récent      : %s (%d)\n", new->titre, new->annee);

    fprintf(f, "\n Livres par catégorie :\n");

    char categories[100][CATEGORIE_LEN];
    int nbCat = 0;

    for (size_t i = 0; i < b->nb; i++) {
        int dejaVu = 0;
        for (int j = 0; j < nbCat; j++) {
            if (strcmp(categories[j], b->livres[i].categorie) == 0) {
                dejaVu = 1;
                break;
            }
        }
        if (!dejaVu) {
            strcpy(categories[nbCat], b->livres[i].categorie);
            nbCat++;

            int count = compterLivresParCategorie(b, b->livres[i].categorie);
            fprintf(f, "   %-15s : %d\n", b->livres[i].categorie, count);
        }
    }

    fprintf(f, "\nRapport généré le : %04d-%02d-%02d %02d:%02d:%02d\n",
        1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);

    fclose(f);
    return STATUT_OK;
}