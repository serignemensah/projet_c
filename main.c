#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "partie_livres/bibliotheque.h"
#include "partie_livres/recherche.h"
#include "partie_livres/test.h"
#include "partie_livres/statistiques.h"
#include"partie_livres/utilisateur.h"
#include"partie_livres/emprunt.h"


void menu_principal(void) {

}

int main(void) {
    Bibliotheque b = {0};
    BaseUtilisateurs base = {0};
    BaseEmprunts base_emprunts = { .nbEmprunts = 0 };

    initBibliotheque(&b);
    chargerBibliotheque(&b, "livres_table.txt");

    // Charger utilisateurs
    chargerUtilisateurs(&base, "test_utilisateurs.csv");

    int choix = 20;

    do {
        printf("\n========================================\n");
        printf("           MENU PRINCIPAL\n");
        printf("========================================\n");
        printf(" 1. Gestion des livres\n");
        printf(" 2. Gestion des utilisateurs\n");
        printf(" 3. Gestion des emprunts\n");
        printf(" 0. Quitter\n");
        printf("----------------------------------------\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Entree invalide.\n");
            continue;
        }
        getchar(); // consomme \n

        switch (choix) {

        case 1:
            menu_livres(&b);
            break;

        case 2:
            menu_recherche(&base);
            break;

            case 3:
                afficherMenuEmprunt(&base_emprunts);
              break;
        case 0:
            printf("Au revoir !\n");
            sauvegarderBibliothequeTable(&b, "livres_table.txt");
            sauvegarderUtilisateurs(&base, "test_utilisateurs.csv");
            sauvegarderEmprunts(&base_emprunts,"test_emprunts.csv");
            break;

        default:
            printf("Choix invalide.\n");
        }

    } while (choix != 0);

    freeBibliotheque(&b);
    return 0;
}














