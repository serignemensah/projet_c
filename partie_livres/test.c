//
// Created by marie on 03/11/2025.
//

#include "test.h"
#include <stdio.h>
#include <string.h>
#include "bibliotheque.h"
#include "recherche.h"

// Déclarations des fonctions CRUD (si pas déjà dans un .h)
StatutLivre initBibliotheque(Bibliotheque *b);
StatutLivre ajouterLivre(Bibliotheque *b, const Livre *l);
StatutLivre supprimerLivre(Bibliotheque *b, const char *isbn);
StatutLivre modifierLivre(Bibliotheque *b, const char *isbn, const UpdateLivre *u);
void freeBibliotheque(Bibliotheque *b);

#define ASSERT(msg, expr) \
    printf("%s : %s\n", msg, (expr) ? " OK" : " FAIL")

int test_ajout() {
    printf("\n==== Test ajouterLivre ====\n");
    Bibliotheque b;
    initBibliotheque(&b);

    Livre l1 = {"123", "Test", "Auteur", 2020, "Roman", LIVRE_DISPONIBLE};
    Livre l2 = {"123", "Doublon", "Auteur2", 2021, "SciFi", LIVRE_DISPONIBLE};

    ASSERT("Ajout livre normal", ajouterLivre(&b, &l1) == STATUT_OK);
    ASSERT("1 livre dans bibliothèque", b.nb == 1);
    ASSERT("Doublon ISBN refusé", ajouterLivre(&b, &l2) == STATUT_DOUBLON_ISBN);
    ASSERT("Toujours 1 livre", b.nb == 1);

    freeBibliotheque(&b);
}

int test_suppression() {
    printf("\n==== Test supprimerLivre ====\n");
    Bibliotheque b;
    initBibliotheque(&b);

    Livre l = {"111", "Livre A", "A", 1990, "Roman", LIVRE_DISPONIBLE};
    ajouterLivre(&b, &l);

    ASSERT("Suppression OK", supprimerLivre(&b, "111") == STATUT_OK);
    ASSERT("Bibliothèque vide", b.nb == 0);
    ASSERT("Suppression livre inexistant", supprimerLivre(&b, "000") == STATUT_NON_TROUVE);

    freeBibliotheque(&b);
}

int test_modification() {
    printf("\n==== Test modifierLivre ====\n");
    Bibliotheque b;
    initBibliotheque(&b);

    Livre l = {"222", "Titre", "Auteur", 2000, "IDK", LIVRE_DISPONIBLE};
    ajouterLivre(&b, &l);

    UpdateLivre u = {0};
    u.mask = UPD_TITRE | UPD_ANNEE;
    strcpy(u.new_titre, "Nouveau");
    u.new_annee = 2023;

    ASSERT("Modification OK", modifierLivre(&b, "222", &u) == STATUT_OK);
    ASSERT("Titre modifié", strcmp(b.livres[0].titre, "Nouveau") == 0);
    ASSERT("Année modifiée", b.livres[0].annee == 2023);
    ASSERT("Livre absent => erreur", modifierLivre(&b, "333", &u) == STATUT_NON_TROUVE);

    freeBibliotheque(&b);
}

int test_recherche() {
    printf("\n==== Test recherche ====\n");
    Bibliotheque b;
    initBibliotheque(&b);

    Livre l1 = {"1", "C Programming", "Kernighan", 1978, "Programmation", LIVRE_DISPONIBLE};
    Livre l2 = {"2", "Harry Potter", "Rowling", 1997, "Roman", LIVRE_DISPONIBLE};
    ajouterLivre(&b, &l1);
    ajouterLivre(&b, &l2);

    printf("Recherche titre = 'C'\n");
    rechercherLivreParTitre(&b, "C");

    printf("Recherche auteur = 'Row'\n");
    rechercherLivreParAuteur(&b, "Row");

    printf("Recherche ISBN = '1'\n");
    rechercherLivreParISBN(&b, "1");

    printf("Recherche catégorie = 'Roman'\n");
    rechercherLivreParCategorie(&b, "Roman");

    printf("Recherche inexistante\n");
    rechercherLivreParTitre(&b, "xyz");

    freeBibliotheque(&b);
}
