#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "partie_livres/recherche.h"
#include "partie_livres/emprunt.h"
#include "partie_livres/utilisateur.h"

int main() {

    Bibliotheque b;
    BaseUtilisateurs bu;
    BaseEmprunts be;
    Utilisateur u;

    int r = ROLE_UTILISATEUR;

    initBibliotheque(&b);
    bu.nbUtilisateurs = 0;
    be.nbEmprunts = 0;

    chargerToutesDonnees(&b, &bu, &be);

    /* ========= AUTHENTIFICATION ========= */
    memset(&u, 0, sizeof(u));
    r = Admin(&u);   // fournir le pointeur utilisateur

    int choix;

    do {
        printf("\n===== MENU PRINCIPAL =====\n");


        printf("1. Gestion des utilisateurs\n");
        printf("2. Gestion des livres\n");
        printf("3. Gestion des emprunts\n");
        printf("0. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {

            case 1:
                    menu_recherche(&bu,r);

                break;

            case 2:
                menu_livres(&b, r);
                break;

            case 3:
                afficherMenuEmprunt(&b, &bu, &be, r);
                break;

            case 0:
                break;

            default:
                printf("Entrée invalide.\n");
        }

    } while (choix != 0);

    sauvegarderToutesDonnees(&b, &bu, &be);

    return 0;
}
