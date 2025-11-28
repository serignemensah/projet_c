#include <stdio.h>

#include "emprunt.h"
#include "utilisateur.h"
#include <stdio.h>
#include <string.h>
#include "emprunt.h"

// Vérifie si un emprunt existe
int existeEmprunt(BaseEmprunts *base_emprunts, int id) {
    for (int i = 0; i < base_emprunts->nbEmprunts; i++) {
        if (base_emprunts->emprunts[i].idEmprunt == id)
            return 1;
    }
    return 0;
}


// Ajouter un emprunt
StatutEmprunt ajouterEmprunt(BaseEmprunts *base_emprunts, Emprunt e) {

    if (base_emprunts->nbEmprunts >= MAX_EMPRUNTS)
        return BASE_EMPRUNT_PLEINE;

    if (existeEmprunt(base_emprunts, e.idEmprunt))
        return EMPRUNT_EXISTE;

    base_emprunts->emprunts[base_emprunts->nbEmprunts] = e;
    base_emprunts->nbEmprunts++;

    return EMPRUNT_EXISTE;  // tu peux renvoyer OK si tu veux
}

// Modifier un emprunt
StatutEmprunt modifierEmprunt(BaseEmprunts *base_emprunts, Emprunt upd) {

    for (int i = 0; i < base_emprunts->nbEmprunts; i++) {
        if (base_emprunts->emprunts[i].idEmprunt == upd.idEmprunt) {

            base_emprunts->emprunts[i] = upd;
            return EMPRUNT_EXISTE;
        }
    }
    return EMPRUNT_INEXISTANT;
}

// Supprimer un emprunt
StatutEmprunt supprimerEmpruntParID(BaseEmprunts *base_emprunts, int id) {

    for (int i = 0; i < base_emprunts->nbEmprunts; i++) {
        if (base_emprunts->emprunts[i].idEmprunt == id) {

            base_emprunts->emprunts[i] = base_emprunts->emprunts[base_emprunts->nbEmprunts - 1];
            base_emprunts->nbEmprunts--;

            return EMPRUNT_EXISTE;
        }
    }
    return EMPRUNT_INEXISTANT;
}
// Rechercher un emprunt
Emprunt* rechercherEmpruntParID(BaseEmprunts *base_emprunts, int id) {

    for (int i = 0; i < base_emprunts->nbEmprunts; i++) {
        if (base_emprunts->emprunts[i].idEmprunt == id)
            return &base_emprunts->emprunts[i];
    }
    return NULL;
}
void trim(char *str);

void afficherMenuEmprunt(BaseEmprunts *base_emprunts) {
    int choix;

    do {
        printf("\n========= MENU GESTION DES EMPRUNTS =========\n");
        printf("1. Ajouter un emprunt\n");
        printf("2. Modifier un emprunt\n");
        printf("3. Supprimer un emprunt\n");
        printf("4. Rechercher un emprunt par ID\n");
        printf("5. Afficher tous les emprunts\n");
        printf("0. Retour au menu principal\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        getchar();

        switch (choix) {

            /*----------------------------------------------------
             * 1. AJOUTER UN EMPRUNT
             ----------------------------------------------------*/
            case 1: {
                Emprunt e;

                printf("ID de l'emprunt : ");
                scanf("%d", &e.idEmprunt); getchar();

                printf("ID Utilisateur : ");
                scanf("%d", &e.idUtilisateur); getchar();

                printf("ID Livre : ");
                scanf("%d", &e.idLivre); getchar();

                printf("Date d'emprunt (YYYY-MM-DD) : ");
                fgets(e.dateEmprunt, sizeof(e.dateEmprunt), stdin);
                trim(e.dateEmprunt);

                printf("Date de retour (YYYY-MM-DD) : ");
                fgets(e.dateRetour, sizeof(e.dateRetour), stdin);
                trim(e.dateRetour);

                e.rendu = 0; // par défaut non rendu

                if (ajouterEmprunt(base_emprunts, e)) {
                    printf("✔ Emprunt ajouté.\n");
                }
                else
                    printf("❌ Erreur ou emprunt existe déjà.\n");

            } break;

            /*----------------------------------------------------
             * 2. MODIFIER UN EMPRUNT
             ----------------------------------------------------*/
            case 2: {
                int id;
                printf("ID de l'emprunt à modifier : ");
                scanf("%d", &id); getchar();

                Emprunt *e = rechercherEmpruntParID(base_emprunts, id);

                if (!e) {
                    printf("❌ Emprunt introuvable.\n");
                    break;
                }

                Emprunt upd = *e;

                printf("Nouvelle date d'emprunt (%s) : ", e->dateEmprunt);
                fgets(upd.dateEmprunt, sizeof(upd.dateEmprunt), stdin);
                trim(upd.dateEmprunt);

                printf("Nouvelle date de retour (%s) : ", e->dateRetour);
                fgets(upd.dateRetour, sizeof(upd.dateRetour), stdin);
                trim(upd.dateRetour);

                printf("Rendu ? (0 = non, 1 = oui) : ");
                scanf("%d", &upd.rendu); getchar();

                if (modifierEmprunt(base_emprunts, upd))
                    printf("✔ Modification réussie.\n");
                else
                    printf("❌ Erreur lors de la modification.\n");

            } break;

            /*----------------------------------------------------
             * 3. SUPPRIMER UN EMPRUNT
             ----------------------------------------------------*/
            case 3: {
                int id;
                printf("ID de l'emprunt à supprimer : ");
                scanf("%d", &id); getchar();

                if (supprimerEmpruntParID(base_emprunts, id))
                    printf("✔ Emprunt supprimé.\n");
                else
                    printf("❌ Emprunt introuvable.\n");

            } break;

            /*----------------------------------------------------
             * 4. RECHERCHER PAR ID
             ----------------------------------------------------*/
            case 4: {
                int id;
                printf("ID à rechercher : ");
                scanf("%d", &id); getchar();

                Emprunt *e = rechercherEmpruntParID(base_emprunts, id);

                if (!e) {
                    printf("❌ Emprunt introuvable\n");
                } else {
                    printf("\n--- EMPRUNT %d ---\n", e->idEmprunt);
                    printf("Utilisateur : %d\n", e->idUtilisateur);
                    printf("Livre       : %d\n", e->idLivre);
                    printf("Date emprunt: %s\n", e->dateEmprunt);
                    printf("Date retour : %s\n", e->dateRetour);
                    printf("Rendu       : %s\n", e->rendu ? "oui" : "non");
                }
            } break;

            /*----------------------------------------------------
             * 5. AFFICHER TOUS LES EMPRUNTS
             ----------------------------------------------------*/
            case 5:
                afficherMenuEmprunt(base_emprunts);
                break;

            case 0:

                printf("Retour au menu principal...\n");
                break;

            default:
                printf("❌ Choix invalide\n");
        }

    } while (choix != 0);
}

StatutEmprunt sauvegarderEmprunts(BaseEmprunts *base_emprunts, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return BASE_EMPRUNT_PLEINE; // impossible d'ouvrir le fichier

    for (int i = 0; i < base_emprunts->nbEmprunts; i++) {
        Emprunt e = base_emprunts->emprunts[i];
        fprintf(f, "%d,%d,%d,%s,%s,%d\n",
                e.idEmprunt,
                e.idUtilisateur,
                e.idLivre,
                e.dateEmprunt,
                e.dateRetour,
                e.rendu);
    }

    fclose(f);
    return EMPRUNT_EXISTE;
}