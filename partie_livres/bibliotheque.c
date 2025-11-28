#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "recherche.h"


/* Bornes valides de l'année */
#define ANNEE_MIN 1450
#define ANNEE_MAX 2100
#define CAPACITE_INITIALE 10

/* -----------------------------------------------------
 *                 HELPERS INTERNES
 * ----------------------------------------------------- */

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

static ssize_t index_par_isbn(const Bibliotheque *b, const char *isbn) {
    for (size_t i = 0; i < b->nb; ++i) {
        if (strcmp(b->livres[i].isbn, isbn) == 0)
            return (ssize_t)i;
    }
    return -1;
}

static StatutLivre ensure_capacity(Bibliotheque *b, size_t besoin) {
    if (b->capacite >= besoin) return STATUT_OK;

    size_t nouv = (b->capacite == 0) ? 8 : b->capacite * 2;
    while (nouv < besoin) nouv *= 2;

    void *p = realloc(b->livres, nouv * sizeof(Livre));
    if (!p) return STATUT_MEMOIRE;

    b->livres = (Livre*)p;
    b->capacite = nouv;
    return STATUT_OK;
}

/* -----------------------------------------------------
 *              FONCTIONS PRINCIPALES
 * ----------------------------------------------------- */

StatutLivre initBibliotheque(Bibliotheque *b) {
    b->livres = malloc(CAPACITE_INITIALE * sizeof(Livre));
    if (!b->livres) return STATUT_MEMOIRE;

    b->nb        = 0;
    b->capacite  = CAPACITE_INITIALE;
    return STATUT_OK;
}

/* -------------------- AJOUT -------------------- */

StatutLivre ajouterLivre(Bibliotheque *b, const Livre *livre) {
    if (!b || !livre) return STATUT_ENTREE_INVALIDE;
    if (isbn_est_vide(livre->isbn)) return STATUT_ENTREE_INVALIDE;
    if (!annee_valide(livre->annee)) return STATUT_ENTREE_INVALIDE;

    /* Vérifier si un livre avec le même ISBN existe */
    int idx = index_par_isbn(b, livre->isbn);
    if (idx >= 0) {
        // AJOUT D’EXEMPLAIRES
        Livre *exist = &b->livres[idx];
        exist->nb_exemplaires_total       += livre->nb_exemplaires_total;
        exist->nb_exemplaires_disponibles += livre->nb_exemplaires_disponibles;

        if (exist->nb_exemplaires_disponibles > 0)
            exist->dispo = LIVRE_DISPONIBLE;

        return STATUT_OK;
    }

    /* Si nouveau livre → agrandir tableau */
    StatutLivre st = ensure_capacity(b, b->nb + 1);
    if (st != STATUT_OK) return st;

    /* Copier proprement */
    b->livres[b->nb] = *livre;

    if (b->livres[b->nb].nb_exemplaires_total <= 0)
        b->livres[b->nb].nb_exemplaires_total = 1;

    if (b->livres[b->nb].nb_exemplaires_disponibles < 0 ||
        b->livres[b->nb].nb_exemplaires_disponibles >
        b->livres[b->nb].nb_exemplaires_total)
    {
        b->livres[b->nb].nb_exemplaires_disponibles =
        b->livres[b->nb].nb_exemplaires_total;
    }

    b->nb++;
    return STATUT_OK;
}

/* -------------------- SUPPRESSION -------------------- */

StatutLivre supprimerLivre(Bibliotheque *b, const char *isbn) {
    if (!b || isbn_est_vide(isbn)) return STATUT_ENTREE_INVALIDE;

    ssize_t idx = index_par_isbn(b, isbn);
    if (idx < 0) return STATUT_NON_TROUVE;

    size_t restants = b->nb - idx - 1;

    if (restants > 0)
        memmove(&b->livres[idx], &b->livres[idx + 1], restants * sizeof(Livre));

    b->nb--;
    return STATUT_OK;
}

/* -------------------- MODIFICATION -------------------- */

StatutLivre modifierLivre(Bibliotheque *b, const char *isbn, const UpdateLivre *u) {
    if (!b || isbn_est_vide(isbn) || !u) return STATUT_ENTREE_INVALIDE;

    ssize_t idx = index_par_isbn(b, isbn);
    if (idx < 0) return STATUT_NON_TROUVE;

    Livre *L = &b->livres[idx];

    if (u->mask & UPD_TITRE) {
        strncpy(L->titre, u->new_titre, TITRE_LEN - 1);
        L->titre[TITRE_LEN - 1] = '\0';
    }

    if (u->mask & UPD_AUTEUR) {
        strncpy(L->auteur, u->new_auteur, AUTEUR_LEN - 1);
        L->auteur[AUTEUR_LEN - 1] = '\0';
    }

    if (u->mask & UPD_ANNEE) {
        if (!annee_valide(u->new_annee))
            return STATUT_ENTREE_INVALIDE;
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

/* -------------------- EXEMPLAIRES -------------------- */

int getNbExemplairesDisponibles(const Bibliotheque *b, const char *isbn) {
    int idx = index_par_isbn(b, isbn);
    if (idx < 0) return -1;

    return b->livres[idx].nb_exemplaires_disponibles;
}

StatutLivre emprunterExemplaire(Bibliotheque *b, const char *isbn) {
    int idx = index_par_isbn(b, isbn);
    if (idx < 0) return STATUT_LIVRE_INEXISTANT;

    Livre *l = &b->livres[idx];
    if (l->nb_exemplaires_disponibles <= 0)
        return STATUT_PLUS_DISPONIBLE;

    l->nb_exemplaires_disponibles--;

    if (l->nb_exemplaires_disponibles == 0)
        l->dispo = LIVRE_EMPRUNTE;

    return STATUT_OK;
}

StatutLivre retournerExemplaire(Bibliotheque *b, const char *isbn) {
    int idx = index_par_isbn(b, isbn);
    if (idx < 0) return STATUT_LIVRE_INEXISTANT;

    Livre *l = &b->livres[idx];

    if (l->nb_exemplaires_disponibles < l->nb_exemplaires_total)
        l->nb_exemplaires_disponibles++;

    if (l->nb_exemplaires_disponibles > 0)
        l->dispo = LIVRE_DISPONIBLE;

    return STATUT_OK;
}

/* -------------------- AFFICHAGE -------------------- */

void afficherEnteteTable() {
    printf("\n---------------------------------------------------------------------------------------------------------------\n");
    printf("| ISBN          | Titre               | Auteur          | Categorie   | Annee | Tot  | Disp | Etat      |\n");
    printf("---------------------------------------------------------------------------------------------------------------\n");
}

static const char *etat_str[] = {
    "DISPO", "EMPRUNTE", "RESERVE", "PERDU", "HS"
};

void afficherTousLesLivresTable(const Bibliotheque *b) {
    if (b->nb == 0) {
        printf("Aucun livre dans la bibliothèque.\n");
        return;
    }

    afficherEnteteTable();

    for (size_t i = 0; i < b->nb; i++) {
        const Livre *l = &b->livres[i];

        printf("| %-13s | %-20s | %-15s | %-12s | %-5d | %-5d | %-5d | %-9s |\n",
               l->isbn,
               l->titre,
               l->auteur,
               l->categorie,
               l->annee,
               l->nb_exemplaires_total,
               l->nb_exemplaires_disponibles,
               etat_str[l->dispo]
        );
    }

    printf("---------------------------------------------------------------------------------------------------------------\n");
}

/* -------------------- SAUVEGARDE TABLE -------------------- */

StatutLivre sauvegarderBibliotheque(const Bibliotheque *b, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return STATUT_INTERNE;

    for (size_t i = 0; i < b->nb; i++) {
        const Livre *l = &b->livres[i];

        fprintf(f, "%s|%s|%s|%d|%s|%d|%d|%d\n",
                l->isbn,
                l->titre,
                l->auteur,
                l->annee,
                l->categorie,
                l->dispo,
                l->nb_exemplaires_total,
                l->nb_exemplaires_disponibles);
    }

    fclose(f);
    return STATUT_OK;
}

/* Pas encore implémenté → OK */
StatutLivre chargerBibliotheque(Bibliotheque *b, const char *filename) {
    return STATUT_OK;
}

/* -------------------- LIBÉRATION -------------------- */

void freeBibliotheque(Bibliotheque *b) {
    free(b->livres);
    b->livres   = NULL;
    b->nb       = 0;
    b->capacite = 0;
}
void menu_livres(Bibliotheque *b)
{
    int choix;
    char buffer[256];
    char isbn[ISBN_LEN];
    char input[256];
    int annee = 0;

    do {
        printf("\n=========== MENU LIVRES ===========\n");
        printf(" 1. Ajouter un livre\n");
        printf(" 2. Supprimer un livre\n");
        printf(" 3. Modifier un livre\n");
        printf(" 4. Afficher tous les livres\n");
        printf(" 5. Afficher exemplaires disponibles\n");
        printf(" 6. Rechercher livre par titre\n");
        printf(" 7. Rechercher livre par auteur\n");
        printf(" 8. Rechercher livre par ISBN\n");
        printf(" 9. Rechercher livre par catégorie\n");
        printf("10. Afficher livres triés par titre\n");
        printf("11. Afficher livres triés par année\n");
        printf("12. Afficher livres d’une année donnée\n");
        printf("13. Afficher disponibilité d’un livre\n");
        printf(" 0. Quitter\n");
        printf("===================================\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix) != 1) {
            while (getchar() != '\n');
            printf("Entrée invalide.\n");
            continue;
        }
        getchar();

        Livre l;
        UpdateLivre u;
        memset(&u, 0, sizeof(u));

        switch (choix)
        {
        case 1:
            printf("ISBN : ");
            fgets(l.isbn, sizeof(l.isbn), stdin);

            printf("Titre : ");
            fgets(l.titre, sizeof(l.titre), stdin);

            printf("Auteur : ");
            fgets(l.auteur, sizeof(l.auteur), stdin);

            printf("Categorie : ");
            fgets(l.categorie, sizeof(l.categorie), stdin);

            printf("Année : ");
            scanf("%d", &l.annee);
            getchar();

            int nb_ex;
            printf("Nombre d'exemplaires : ");
            scanf("%d", &nb_ex);
            getchar();

            if (nb_ex <= 0) nb_ex = 1;

            l.isbn[strcspn(l.isbn, "\n")] = 0;
            l.titre[strcspn(l.titre, "\n")] = 0;
            l.auteur[strcspn(l.auteur, "\n")] = 0;
            l.categorie[strcspn(l.categorie, "\n")] = 0;

            l.nb_exemplaires_total       = nb_ex;
            l.nb_exemplaires_disponibles = nb_ex;
            l.dispo                      = LIVRE_DISPONIBLE;

            if (ajouterLivre(b, &l) == STATUT_OK)
                printf(" Livre ajouté.\n");
            else
                printf(" Erreur lors de l’ajout.\n");
            break;

        case 2:
            printf("ISBN du livre à supprimer : ");
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = 0;

            if (supprimerLivre(b, isbn) == STATUT_OK)
                printf(" Livre supprimé.\n");
            else
                printf(" Livre non trouvé.\n");
            break;

        case 3:
            printf("ISBN du livre à modifier : ");
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = 0;

            printf("Nouveau titre (laisser vide = ne pas changer) : ");
            fgets(buffer, sizeof(buffer), stdin);
            if (buffer[0] != '\n') {
                buffer[strcspn(buffer, "\n")] = 0;
                strcpy(u.new_titre, buffer);
                u.mask |= UPD_TITRE;
            }

            printf("Nouvel auteur (laisser vide = ne pas changer) : ");
            fgets(buffer, sizeof(buffer), stdin);
            if (buffer[0] != '\n') {
                buffer[strcspn(buffer, "\n")] = 0;
                strcpy(u.new_auteur, buffer);
                u.mask |= UPD_AUTEUR;
            }

            printf("Nouvelle année (0 = ne pas changer) : ");
            scanf("%d", &annee);
            getchar();
            if (annee != 0) {
                u.new_annee = annee;
                u.mask |= UPD_ANNEE;
            }

            printf("Nouvelle catégorie (laisser vide = ne pas changer) : ");
            fgets(buffer, sizeof(buffer), stdin);
            if (buffer[0] != '\n') {
                buffer[strcspn(buffer, "\n")] = 0;
                strcpy(u.new_categorie, buffer);
                u.mask |= UPD_CATEGORIE;
            }

            if (modifierLivre(b, isbn, &u) == STATUT_OK)
                printf(" Livre modifié.\n");
            else
                printf(" Livre non trouvé.\n");
            break;

        case 4:
            afficherTousLesLivresTable(b);
            break;

        case 5:
            printf("ISBN : ");
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = 0;

            int dispo = getNbExemplairesDisponibles(b, isbn);
            if (dispo >= 0)
                printf(" Exemplaires disponibles : %d\n", dispo);
            else
                printf(" Livre introuvable.\n");
            break;

        /* ------------------ FONCTIONS RAJOUTÉES ------------------ */

        case 6:
            printf("Titre à rechercher : ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            rechercherLivreParTitre(b, input);
            break;

        case 7:
            printf("Auteur à rechercher : ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            rechercherLivreParAuteur(b, input);
            break;

        case 8:
            printf("ISBN : ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            rechercherLivreParISBN(b, input);
            break;

        case 9:
            printf("Catégorie : ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            rechercherLivreParCategorie(b, input);
            break;

        case 10:
            afficherLivresParTitre(b);
            break;

        case 11:
            afficherLivresParAnnee(b);
            break;

        case 12:
            printf("Année : ");
            scanf("%d", &annee);
            getchar();
            afficherLivresDuneAnnee(b, annee);
            break;

        case 13:
            printf("ISBN : ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            afficherDisponibiliteLivre(b, input);
            break;

        case 0:
            printf("Retour au menu principal...\n");
            break;

        default:
            printf("Choix invalide.\n");
        }

    } while (choix != 0);
}
