#include <stdio.h>
#include "partie_livres/bibliotheque.h"
#include "partie_livres/emprunt.h"
#include "partie_livres/utilisateur.h"
#include <stdlib.h>
int main() {

    Bibliotheque b;
    BaseUtilisateurs bu;
    BaseEmprunts be;

    initBibliotheque(&b);
    bu.nbUtilisateurs = 0;
    be.nbEmprunts = 0;

    chargerToutesDonnees(&b, &bu, &be);

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
            case 1: menu_recherche(&bu); break;
            case 2: menu_livres(&b); break;
            case 3: afficherMenuEmprunt(&b, &bu, &be); break;
        }

    } while (choix != 0);

    sauvegarderToutesDonnees(&b, &bu, &be);
    free(b.livres);

    return 0;
}
