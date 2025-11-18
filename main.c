#include <stdio.h>
#include <string.h>
#include "partie_livres/bibliotheque.h"
#include "partie_livres/recherche.h"
#include "partie_livres/test.h"
#include "partie_livres/statistiques.h"
#include <stdlib.h>

StatutLivre ajouterLivre(Bibliotheque *b, const Livre *l);
void freeBibliotheque(Bibliotheque *b);


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



int main() {
    system("cls");

    Bibliotheque b = {0};
    int choix;
    char buffer[200];
    initBibliotheque(&b);
    chargerBibliotheque(&b, "livres.txt");
    do {
        menu();
        scanf("%d", &choix);
        getchar(); // enlever le \n restant

        char isbn[ISBN_LEN], titre[TITRE_LEN], auteur[AUTEUR_LEN], categorie[CATEGORIE_LEN];
        int annee;
        Livre l;
        UpdateLivre u = {0};

        switch (choix) {

        case 1: // Ajouter
            printf("ISBN : "); fgets(l.isbn, sizeof(l.isbn), stdin);
            printf("Titre : "); fgets(l.titre, sizeof(l.titre), stdin);
            printf("Auteur : "); fgets(l.auteur, sizeof(l.auteur), stdin);
            printf("Categorie : "); fgets(l.categorie, sizeof(l.categorie), stdin);
            printf("Annee : "); scanf("%d", &l.annee); getchar();
            l.dispo = LIVRE_DISPONIBLE;

            // enlever '\n'
            l.isbn[strcspn(l.isbn, "\n")] = 0;
            l.titre[strcspn(l.titre, "\n")] = 0;
            l.auteur[strcspn(l.auteur, "\n")] = 0;
            l.categorie[strcspn(l.categorie, "\n")] = 0;

            if (ajouterLivre(&b, &l) == STATUT_OK)
                printf(" Livre ajouté !\n");
            else
                printf(" Erreur ajout livre !\n");

            printf("\nAppuyez sur Entrée pour continuer...");
            getchar();

            break;

        case 2: // Supprimer
            printf("ISBN du livre à supprimer : ");
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = 0;
            if (supprimerLivre(&b, isbn) == STATUT_OK)
                printf(" Livre supprimé !\n");
            else
                printf(" Livre non trouvé.\n");
            break;

        case 3: // Modifier
            printf("ISBN du livre à modifier : ");
            fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = 0;

            printf("Nouveau titre (laisser vide si rien) : ");
            fgets(buffer, sizeof(buffer), stdin);
            if (buffer[0] != '\n') {
                buffer[strcspn(buffer, "\n")] = 0;
                u.mask |= UPD_TITRE;
                strcpy(u.new_titre, buffer);
            }

            printf("Nouvel auteur (laisser vide si rien) : ");
            fgets(buffer, sizeof(buffer), stdin);
            if (buffer[0] != '\n') {
                buffer[strcspn(buffer, "\n")] = 0;
                u.mask |= UPD_AUTEUR;
                strcpy(u.new_auteur, buffer);
            }

            printf("Nouvelle année (0 si pas changer) : ");
            scanf("%d", &annee); getchar();
            if (annee != 0) {
                u.mask |= UPD_ANNEE;
                u.new_annee = annee;
            }

            printf("Nouvelle catégorie (laisser vide si rien) : ");
            fgets(buffer, sizeof(buffer), stdin);
            if (buffer[0] != '\n') {
                buffer[strcspn(buffer, "\n")] = 0;
                u.mask |= UPD_CATEGORIE;
                strcpy(u.new_categorie, buffer);
            }

            if (modifierLivre(&b, isbn, &u) == STATUT_OK)
                printf(" Livre modifié\n");
            else
                printf(" Livre non trouvé\n");
            break;

        case 4:
            printf("Titre : "); fgets(buffer, 200, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            rechercherLivreParTitre(&b, buffer);
            break;

        case 5:
            printf("Auteur : "); fgets(buffer, 200, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            rechercherLivreParAuteur(&b, buffer);
            break;

        case 6:
            printf("ISBN : "); fgets(buffer, 200, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            rechercherLivreParISBN(&b, buffer);
            break;

        case 7:
            printf("Categorie : "); fgets(buffer, 200, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            rechercherLivreParCategorie(&b, buffer);
            break;

        case 8: afficherLivresParTitre(&b); break;
        case 9:
            {
                printf("Annee : "); scanf("%d", &l.annee); getchar();
                afficherLivresDuneAnnee(&b, l.annee);
                break;
            }

        case 10:
            printf("ISBN : "); fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = 0;
            afficherDisponibiliteLivre(&b, isbn);
            break;

        case 11:
            printf("ISBN : "); fgets(isbn, sizeof(isbn), stdin);
            isbn[strcspn(isbn, "\n")] = 0;
            printf("Nouvel état (0=dispo 1=emprunté 2=res. 3=perdu 4=HS) : ");
            scanf("%d", &annee); getchar();
            setDisponibiliteLivre(&b, isbn, (Disponibilite)annee);
            break;

        case 12 :
            int annee;
            printf("Entrez une année : ");
            scanf("%d", &annee);
            getchar();
            afficherLivresDuneAnnee(&b, annee);
            break;

        case 13 :
            {
                afficherStatistiquesBibliotheque(&b);
                break;
            }

        case 14:
            if (genererRapportStatistiques(&b, "rapport_stats.txt") == STATUT_OK)
                printf(" Rapport exporté dans rapport_stats.txt \n");
            else
                printf(" Erreur lors de la génération du rapport\n");
            break;

        case 0:
            printf(" Au revoir !\n");
            sauvegarderBibliotheque(&b, "livres.txt");
            printf(" Données sauvegardées.\n");
            break;

        default:
            printf(" Choix invalide\n");
        }

    } while (choix != 0);

    freeBibliotheque(&b);
    return 0;
}


/*
int main() {
    test_ajout();
    test_suppression();
    test_modification();
    test_recherche();

    printf("\n Tests terminés.\n");

    return 0;

}
*/