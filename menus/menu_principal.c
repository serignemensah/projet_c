//
// Created by HASNEM on 19/11/2025.
//

#include "menu_principal.h"
#include <stdio.h>
#include "menu.h"
#include "menu_bibliotheque.h"
#include "menu_livres.h"

// === MENU PRINCIPAL ===
void afficherMenuPrincipal() {
    printf("\n========================================\n");
    printf("               MENU PRINCIPAL\n");
    printf("========================================\n");

    printf(" 1. Gestion de la bibliothèque\n");
    printf(" 2. Gestion des livres\n");
    printf(" 0. Quitter\n");

    printf("----------------------------------------\n");
    printf("Votre choix : ");
}

// === MOTEUR DU MENU PRINCIPAL ===
void demarrerMenuPrincipal(Bibliotheque *b) {
    int choix;

    do {
        afficherMenuPrincipal();
        scanf("%d", &choix);
        getchar(); // évite les problèmes de \n

        switch (choix) {
            case 1:
                // Lance ton menu actuel
                demarrerMenuBibliotheque(b);
                break;

            case 2:
                // Un menu supplémentaire
                demarrerMenuLivres(b);
                break;

            case 0:
                printf("Fermeture du programme...\n");
                break;

            default:
                printf("Choix invalide, veuillez réessayer.\n");
        }

    } while (choix != 0);
}
