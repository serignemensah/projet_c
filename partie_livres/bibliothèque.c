#include <string.h>
#include<stdio.h>
#include <stdlib.h>

#include <ctype.h>

#include "bibliothèque.h"



/* Bornes "raisonnables" pour l'année */

#define ANNEE_MIN 1450

#define ANNEE_MAX 2100
#define CAPACITE_INITIALE 10

/* ---------- Helpers internes ---------- */



static int isbn_est_vide(const char *s) {

    if (!s) return 1;

    while (*s) {

        if (!isspace((unsigned char)*s)) return 0;

        s++;

    }

    return 1;

}



static int annee_valide(int y) {

    return (y >= ANNEE_MIN && y <= ANNEE_MAX);

}



static StatutLivre ensure_capacity(Bibliotheque *b, size_t besoin) {

    if (b->capacite >= besoin) return STATUT_OK;

    size_t nouv = (b->capacite == 0) ? 8 : b->capacite * 2;

    while (nouv < besoin) nouv *= 2;



    void *p = realloc(b->livres, nouv * sizeof(Livre));

    if (!p) return STATUT_MEMOIRE;

    b->livres   = (Livre*)p;

    b->capacite = nouv;

    return STATUT_OK;

}

static ssize_t index_par_isbn(const Bibliotheque *b, const char *isbn) {

    for (size_t i = 0; i < b->nb; ++i) {

        if (strcmp(b->livres[i].isbn, isbn) == 0) return (ssize_t)i;

    }

    return -1;

}



/* ---------- API demandée ---------- */



StatutLivre ajouterLivre(Bibliotheque *b, const Livre *livre) {

    if (!b || !livre) return STATUT_ENTREE_INVALIDE;

    if (isbn_est_vide(livre->isbn)) return STATUT_ENTREE_INVALIDE;

    if (!annee_valide(livre->annee)) return STATUT_ENTREE_INVALIDE;



    /* Unicité ISBN */

    if (index_par_isbn(b, livre->isbn) >= 0) return STATUT_DOUBLON_ISBN;



    /* Capacité */

    StatutLivre st = ensure_capacity(b, b->nb + 1);

    if (st != STATUT_OK) return st;



    /* Copie "propre" de l'objet */

    b->livres[b->nb] = *livre;

    b->nb++;

    return STATUT_OK;

}



StatutLivre supprimerLivre(Bibliotheque *b, const char *isbn) {

    if (!b || isbn_est_vide(isbn)) return STATUT_ENTREE_INVALIDE;



    ssize_t idx = index_par_isbn(b, isbn);

    if (idx < 0) return STATUT_NON_TROUVE;



    /* Déplacement pour conserver l'ordre */

    size_t restants = b->nb - (size_t)idx - 1;

    if (restants > 0) {

        memmove(&b->livres[idx], &b->livres[idx + 1], restants * sizeof(Livre));

    }

    b->nb--;

    return STATUT_OK;

}



StatutLivre modifierLivre(Bibliotheque *b, const char *isbn, const UpdateLivre *u) {

    if (!b || isbn_est_vide(isbn) || !u) return STATUT_ENTREE_INVALIDE;



    ssize_t idx = index_par_isbn(b, isbn);

    if (idx < 0) return STATUT_NON_TROUVE;



    Livre *L = &b->livres[idx];



    /* Appliquer sélectivement selon les bits du mask */

    if (u->mask & UPD_TITRE) {

        strncpy(L->titre, u->new_titre, TITRE_LEN - 1);

        L->titre[TITRE_LEN - 1] = '\0';

    }

    if (u->mask & UPD_AUTEUR) {

        strncpy(L->auteur, u->new_auteur, AUTEUR_LEN - 1);

        L->auteur[AUTEUR_LEN - 1] = '\0';

    }

    if (u->mask & UPD_ANNEE) {

        if (!annee_valide(u->new_annee)) return STATUT_ENTREE_INVALIDE;

        L->annee = u->new_annee;

    }

    if (u->mask & UPD_CATEGORIE) {

        strncpy(L->categorie, u->new_categorie, CATEGORIE_LEN - 1);

        L->categorie[CATEGORIE_LEN - 1] = '\0';

    }

    if (u->mask & UPD_DISPO) {

        L->dispo = u->new_dispo;

    }



    return STATUT_OK;

}
StatutLivre initBibliotheque(Bibliotheque *b) {
    b->livres = malloc(CAPACITE_INITIALE * sizeof(Livre));
    if (!b->livres) return STATUT_MEMOIRE;

    b->nb = 0;
    b->capacite = CAPACITE_INITIALE;
    return STATUT_OK;
}

StatutLivre sauvegarderBibliotheque(const Bibliotheque *b, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return STATUT_INTERNE;

    for (size_t i = 0; i < b->nb; i++) {
        fprintf(f, "%s|%s|%s|%d|%s|%d\n",
            b->livres[i].isbn,
            b->livres[i].titre,
            b->livres[i].auteur,
            b->livres[i].annee,
            b->livres[i].categorie,
            b->livres[i].dispo
        );
    }

    fclose(f);
    return STATUT_OK;
}

StatutLivre chargerBibliotheque(Bibliotheque *b, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return STATUT_OK; // fichier n'existe pas encore, pas grave

    Livre l;
    char ligne[512];

    while (fgets(ligne, sizeof(ligne), f)) {
        sscanf(ligne, "%[^|]|%[^|]|%[^|]|%d|%[^|]|%d",
            l.isbn,
            l.titre,
            l.auteur,
            &l.annee,
            l.categorie,
            (int *)&l.dispo
        );
        ajouterLivre(b, &l);
    }

    fclose(f);
    return STATUT_OK;
}


void freeBibliotheque(Bibliotheque *b) {
    free(b->livres);
    b->livres = NULL;
    b->nb = 0;
    b->capacite = 0;
}
