#include <string.h>
#include<stdio.h>
#include <stdlib.h>

#include <ctype.h>

#include "bibliotheque.h"
#include "statistiques.h"
#include "recherche.h"

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

    /* Vérifier si le livre existe déjà (même ISBN) */
    int idx = index_par_isbn(b, livre->isbn);
    if (idx >= 0) {
        // 🆕 On ne refuse plus : on ajoute des exemplaires
        Livre *exist = &b->livres[idx];

        // On vérifie que les infos de base ne contredisent pas trop
        // (titre/auteur/catégorie, tu peux éventuellement ajouter un test)
        exist->nb_exemplaires_total += livre->nb_exemplaires_total;
        exist->nb_exemplaires_disponibles += livre->nb_exemplaires_disponibles;

        if (exist->nb_exemplaires_disponibles > 0) {
            exist->dispo = LIVRE_DISPONIBLE;
        }

        return STATUT_OK;
    }

    /* Capacité */
    StatutLivre st = ensure_capacity(b, b->nb + 1);
    if (st != STATUT_OK) return st;

    /* Copie "propre" de l'objet */
    b->livres[b->nb] = *livre;

    // sécurité : si tu crées toi-même le Livre sans remplir les champs
    if (b->livres[b->nb].nb_exemplaires_total <= 0) {
        b->livres[b->nb].nb_exemplaires_total = 1;
    }
    if (b->livres[b->nb].nb_exemplaires_disponibles < 0 ||
    b->livres[b->nb].nb_exemplaires_disponibles >
    b->livres[b->nb].nb_exemplaires_total) {
        b->livres[b->nb].nb_exemplaires_disponibles =
        b->livres[b->nb].nb_exemplaires_total;
    }

    if (b->livres[b->nb].nb_exemplaires_disponibles > 0) {
        b->livres[b->nb].dispo = LIVRE_DISPONIBLE;
    }

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
        fprintf(f, "%s|%s|%s|%d|%s|%d|%d|%d\n",
        b->livres[i].isbn,
        b->livres[i].titre,
        b->livres[i].auteur,
        b->livres[i].annee,
        b->livres[i].categorie,
        b->livres[i].dispo,
        b->livres[i].nb_exemplaires_total,
        b->livres[i].nb_exemplaires_disponibles
        );
    }

    fclose(f);
    return STATUT_OK;
}



StatutLivre sauvegarderBibliothequeTable(const Bibliotheque *b, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return STATUT_INTERNE;

    fprintf(f, "---------------------------------------------------------------------------------------------------------------\n");
    fprintf(f, "| ISBN          | Titre               | Auteur          | Categorie   | Annee | Tot  | Disp | Etat      |\n");
    fprintf(f, "---------------------------------------------------------------------------------------------------------------\n");

    const char *etat_str[] = {
        "DISPO", "EMPRUNTE", "RESERVE", "PERDU", "HS"
    };

    for (size_t i = 0; i < b->nb; i++) {
        const Livre *l = &b->livres[i];

        fprintf(f,
            "| %-13s | %-20s | %-15s | %-12s | %-5d | %-5d | %-5d | %-9s |\n",
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

    fprintf(f, "---------------------------------------------------------------------------------------------------------------\n");

    fclose(f);
    return STATUT_OK;
}



StatutLivre chargerBibliotheque(Bibliotheque *b, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return STATUT_OK; // fichier n'existe pas encore, pas grave

    Livre l;
    char ligne[512];

    while (fgets(ligne, sizeof(ligne), f)) {
        int dispo_int = 0;
        int nb_total = 0;
        int nb_dispo = 0;

        // Essayer de lire le nouveau format (8 champs)
        int n = sscanf(ligne, "%[^|]|%[^|]|%[^|]|%d|%[^|]|%d|%d|%d",
        l.isbn,
        l.titre,
        l.auteur,
        &l.annee,
        l.categorie,
        &dispo_int,
        &nb_total,
        &nb_dispo);

        if (n < 6) {
            // ligne invalide, on ignore
            continue;
        }

        l.dispo = (Disponibilite)dispo_int;

        if (n == 6) {
            // Ancien format : pas d'infos sur les exemplaires
            l.nb_exemplaires_total = 1;
            l.nb_exemplaires_disponibles = (l.dispo == LIVRE_DISPONIBLE) ? 1 : 0;
        } else {
            // Nouveau format complet
            l.nb_exemplaires_total = nb_total > 0 ? nb_total : 1;
            l.nb_exemplaires_disponibles = nb_dispo;
            if (l.nb_exemplaires_disponibles < 0)
                l.nb_exemplaires_disponibles = 0;
            if (l.nb_exemplaires_disponibles > l.nb_exemplaires_total)
                l.nb_exemplaires_disponibles = l.nb_exemplaires_total;
        }

        ajouterLivre(b, &l);
    }

    fclose(f);
    return STATUT_OK;
}

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
        l->dispo = LIVRE_EMPRUNTE; // ou un état "non dispo" chez toi

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




void afficherLivreTable(const Livre *l) {
    const char *etat_str[] = {
        "DISPO",
        "EMPRUNTE",
        "RESERVE",
        "PERDU",
        "HS"
    };

    printf("| %-13s | %-20s | %-15s | %-12s | %-5d | %-5d | %-5d | %-9s |\n",
           l->isbn,
           l->titre,
           l->auteur,
           l->categorie,
           l->annee,
           l->nb_exemplaires_total,
           l->nb_exemplaires_disponibles,
           etat_str[l->dispo]);
}



void afficherEnteteTable() {
    printf("\n---------------------------------------------------------------------------------------------------------------\n");
    printf("| ISBN          | Titre               | Auteur          | Categorie   | Annee | Tot  | Disp | Etat      |\n");
    printf("---------------------------------------------------------------------------------------------------------------\n");
}


void afficherTousLesLivresTable(const Bibliotheque *b) {
    if (b->nb == 0) {
        printf("Aucun livre dans la bibliothèque.\n");
        return;
    }

    afficherEnteteTable();

    for (size_t i = 0; i < b->nb; i++) {
        afficherLivreTable(&b->livres[i]);
    }

    printf("---------------------------------------------------------------------------------------------------------------\n");
}


// Fonction menu livres
void menu_livres(Bibliotheque *b) {
    int choix = 20;
    char buffer[256];
    char isbn[ISBN_LEN];
    int annee;
    Livre l;
    UpdateLivre u;

    do {
        // Affichage menu
        printf("\n========================================\n");
        printf(" GESTION BIBLIOTHEQUE\n");
        printf("========================================\n");

        printf("\n--- Gestion des livres ---\n");
        printf(" 1. Ajouter un livre\n");
        printf(" 2. Supprimer un livre\n");
        printf(" 3. Modifier un livre\n");

        printf("\n--- Recherche ---\n");
        printf(" 4. Rechercher par titre\n");
        printf(" 5. Rechercher par auteur\n");
        printf(" 6. Rechercher par ISBN\n");
        printf(" 7. Rechercher par categorie\n");

        printf("\n--- Affichage / Tri ---\n");
        printf(" 8. Afficher livres tries par titre\n");
        printf(" 9. Afficher les livres d'une annee precise\n");

        printf("\n--- Disponibilite / Exemplaires ---\n");
        printf(" 10. Afficher disponibilite et exemplaires d'un livre\n");
        printf(" 11. Modifier disponibilite globale d'un livre\n");
        printf(" 12. Emprunter un exemplaire d'un livre\n");
        printf(" 13. Retourner un exemplaire d'un livre\n");

        printf("\n--- Statistiques ---\n");
        printf(" 14. Afficher statistiques\n");
        printf(" 15. Generer rapport statistiques (.txt)\n");

        printf("\n--- Systeme ---\n");
        printf(" 0. Quitter\n");
        printf("----------------------------------------\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf(" Entree invalide.\n");
            continue;
        }
        getchar(); // consomme \n

        memset(&u, 0, sizeof(u)); // réinitialisation UpdateLivre

        switch (choix) {
            case 1: { // Ajouter un livre
                int nb_ex = 0;
                printf("ISBN : "); fgets(l.isbn, sizeof(l.isbn), stdin);
                printf("Titre : "); fgets(l.titre, sizeof(l.titre), stdin);
                printf("Auteur : "); fgets(l.auteur, sizeof(l.auteur), stdin);
                printf("Categorie : "); fgets(l.categorie, sizeof(l.categorie), stdin);
                printf("Annee : "); scanf("%d", &l.annee); getchar();
                printf("Nombre d'exemplaires : "); scanf("%d", &nb_ex); getchar();
                if (nb_ex <= 0) nb_ex = 1;

                // supprimer \n
                l.isbn[strcspn(l.isbn, "\n")] = 0;
                l.titre[strcspn(l.titre, "\n")] = 0;
                l.auteur[strcspn(l.auteur, "\n")] = 0;
                l.categorie[strcspn(l.categorie, "\n")] = 0;

                l.nb_exemplaires_total = nb_ex;
                l.nb_exemplaires_disponibles = nb_ex;
                l.dispo = LIVRE_DISPONIBLE;

                StatutLivre st = ajouterLivre(b, &l);
                if (st == STATUT_OK) printf(" Livre ajoute !\n");
                else if (st == STATUT_ENTREE_INVALIDE) printf(" Erreur : entree invalide.\n");
                else printf(" Erreur ajout livre (code %d).\n", st);

                printf("\nAppuyez sur Entree pour continuer...");
                fgets(buffer, sizeof(buffer), stdin);
                break;
            }

            case 2: // Supprimer
                printf("ISBN du livre a supprimer : "); fgets(isbn, sizeof(isbn), stdin);
                isbn[strcspn(isbn, "\n")] = 0;
                if (supprimerLivre(b, isbn) == STATUT_OK) printf(" Livre supprime !\n");
                else printf(" Livre non trouve.\n");
                break;

            case 3: { // Modifier
                printf("ISBN du livre a modifier : "); fgets(isbn, sizeof(isbn), stdin);
                isbn[strcspn(isbn, "\n")] = 0;

                memset(&u, 0, sizeof(u));

                printf("Nouveau titre (laisser vide si rien) : "); fgets(buffer, sizeof(buffer), stdin);
                if (buffer[0] != '\n') { buffer[strcspn(buffer, "\n")] = 0; u.mask |= UPD_TITRE; strcpy(u.new_titre, buffer); }
                printf("Nouvel auteur (laisser vide si rien) : "); fgets(buffer, sizeof(buffer), stdin);
                if (buffer[0] != '\n') { buffer[strcspn(buffer, "\n")] = 0; u.mask |= UPD_AUTEUR; strcpy(u.new_auteur, buffer); }
                printf("Nouvelle annee (0 si pas changer) : "); scanf("%d", &annee); getchar();
                if (annee != 0) { u.mask |= UPD_ANNEE; u.new_annee = annee; }
                printf("Nouvelle categorie (laisser vide si rien) : "); fgets(buffer, sizeof(buffer), stdin);
                if (buffer[0] != '\n') { buffer[strcspn(buffer, "\n")] = 0; u.mask |= UPD_CATEGORIE; strcpy(u.new_categorie, buffer); }

                if (modifierLivre(b, isbn, &u) == STATUT_OK) printf(" Livre modifie.\n");
                else printf(" Livre non trouve.\n");
                break;
            }

            case 4: printf("Titre : "); fgets(buffer, sizeof(buffer), stdin); buffer[strcspn(buffer, "\n")] = 0; rechercherLivreParTitre(b, buffer); break;
            case 5: printf("Auteur : "); fgets(buffer, sizeof(buffer), stdin); buffer[strcspn(buffer, "\n")] = 0; rechercherLivreParAuteur(b, buffer); break;
            case 6: printf("ISBN : "); fgets(buffer, sizeof(buffer), stdin); buffer[strcspn(buffer, "\n")] = 0; rechercherLivreParISBN(b, buffer); break;
            case 7: printf("Categorie : "); fgets(buffer, sizeof(buffer), stdin); buffer[strcspn(buffer, "\n")] = 0; rechercherLivreParCategorie(b, buffer); break;
            case 8: afficherTousLesLivresTable(b); break;
            case 9: printf("Annee : "); scanf("%d", &annee); getchar(); afficherLivresDuneAnnee(b, annee); break;

            case 10: {
                printf("ISBN : "); fgets(isbn, sizeof(isbn), stdin); isbn[strcspn(isbn, "\n")] = 0;
                afficherDisponibiliteLivre(b, isbn);
                int dispo_ex = getNbExemplairesDisponibles(b, isbn);
                if (dispo_ex >= 0) printf(" Nombre d'exemplaires disponibles : %d\n", dispo_ex);
                else printf(" Livre introuvable.\n");
                break;
            }

            case 11: {
                int etat; printf("ISBN : "); fgets(isbn, sizeof(isbn), stdin); isbn[strcspn(isbn, "\n")] = 0;
                printf("Nouvel etat (0=dispo 1=emprunte 2=reserve 3=perdu 4=HS) : ");
                scanf("%d", &etat); getchar();
                setDisponibiliteLivre(b, isbn, (Disponibilite)etat);
                break;
            }

            case 12: {
                printf("ISBN du livre a emprunter : "); fgets(isbn, sizeof(isbn), stdin); isbn[strcspn(isbn, "\n")] = 0;
                int dispo_ex = getNbExemplairesDisponibles(b, isbn);
                if (dispo_ex < 0) { printf(" Livre introuvable.\n"); break; }
                printf(" Il reste %d exemplaire(s) disponible(s).\n", dispo_ex);
                if (dispo_ex == 0) { printf(" Aucun exemplaire disponible.\n"); break; }
                printf(" Confirmer l'emprunt ? (o/n) : "); char rep = (char)getchar(); getchar();
                if (rep != 'o' && rep != 'O') { printf(" Emprunt annule.\n"); break; }
                StatutLivre st = emprunterExemplaire(b, isbn);
                if (st == STATUT_OK) printf(" Emprunt enregistre.\n");
                else if (st == STATUT_PLUS_DISPONIBLE) printf(" Plus aucun exemplaire disponible.\n");
                else if (st == STATUT_LIVRE_INEXISTANT) printf(" Livre introuvable.\n");
                else printf(" Erreur lors de l'emprunt (code %d).\n", st);
                break;
            }

            case 13: {
                printf("ISBN du livre a retourner : "); fgets(isbn, sizeof(isbn), stdin); isbn[strcspn(isbn, "\n")] = 0;
                StatutLivre st = retournerExemplaire(b, isbn);
                if (st == STATUT_OK) printf(" Retour enregistre.\n");
                else if (st == STATUT_LIVRE_INEXISTANT) printf(" Livre introuvable.\n");
                else printf(" Erreur lors du retour (code %d).\n", st);
                break;
            }

            case 14:
                {
                    afficherStatistiquesBibliotheque(b);
                    break;
                }

            case 15:
                {
                    if (genererRapportStatistiques(b, "rapport_stats.txt") == STATUT_OK)
                        printf(" Rapport exporte dans rapport_stats.txt\n");
                    else printf(" Erreur lors de la generation du rapport.\n");
                    break;

                }


            case 0:
                {
                    printf(" Au revoir !\n");
                    sauvegarderBibliothequeTable(b, "livres_table.txt");
                    printf(" Donnees sauvegardees.\n");
                    break;
                }


            default: printf("Choix invalide.\n"); break;
        }

    } while (choix != 0);

    freeBibliotheque(b);
}


void freeBibliotheque(Bibliotheque *b) {
    free(b->livres);
    b->livres = NULL;
    b->nb = 0;
    b->capacite = 0;
}
