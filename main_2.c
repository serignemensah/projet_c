#include <stdio.h>
#include "partie_B/recherche_utilisateur.h"

int main() {
    BaseUtilisateurs base;
    base.nbUtilisateurs = 0;

    // Charger la base depuis le fichier
    chargerUtilisateurs(&base, "../test_utilisateurs.csv");

    menu(&base);

    // Sauvegarder la base dans le fichier à la fin
    sauvegarderUtilisateurs(&base, "../test_utilisateurs.csv");

    return 0;
}
