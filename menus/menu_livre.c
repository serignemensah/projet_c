//
// Created by HASNEM on 19/11/2025.
//
#include<stdio.h>
#include "menu_livre.h"
void menu() {
    printf("\n========================================\n");
    printf("           GESTION BIBLIOTHEQUE\n");
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
    printf(" 9. Trier par annee\n");
    printf("10. Livres d'une annee precise\n");

    printf("\n--- Disponibilite ---\n");
    printf("11. Afficher disponibilite d un livre\n");//statut du livre temporairement
    printf("12. Modifier disponibilite d un livre\n");

    printf("\n--- Statistiques ---\n");
    printf("13. Afficher statistiques\n");
    printf("14. Generer rapport statistiques (.txt)\n");

    printf("\n--- Systeme ---\n");
    printf(" 0. Quitter\n");

    printf("----------------------------------------\n");
    printf("Votre choix : ");
}

