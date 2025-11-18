#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "partie_livres/bibliotheque.h"
#include "partie_livres/recherche.h"
#include "partie_livres/test.h"
#include "partie_livres/statistiques.h"

void menu(void) {
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
}

int main(void) {
Bibliotheque b = {0};
int choix = 20;

initBibliotheque(&b);
chargerBibliotheque(&b, "livres.txt");

do {
menu();
if (scanf("%d", &choix) != 1) {
// Entrée non valide -> on vide et on continue
int c;
while ((c = getchar()) != '\n' && c != EOF) {}
printf(" Entree invalide.\n");
continue;
}
getchar(); // consomme le '\n' après le nombre

char buffer[256];
char isbn[ISBN_LEN];
int annee;
Livre l;
UpdateLivre u;
memset(&u, 0, sizeof(u)); // très important pour le mask

switch (choix) {

/* ======================
1. AJOUTER UN LIVRE
====================== */
case 1: {
int nb_ex = 0;

printf("ISBN : ");
fgets(l.isbn, sizeof(l.isbn), stdin);

printf("Titre : ");
fgets(l.titre, sizeof(l.titre), stdin);

printf("Auteur : ");
fgets(l.auteur, sizeof(l.auteur), stdin);

printf("Categorie : ");
fgets(l.categorie, sizeof(l.categorie), stdin);

printf("Annee : ");
scanf("%d", &l.annee);
getchar();

printf("Nombre d'exemplaires : ");
scanf("%d", &nb_ex);
getchar();

if (nb_ex <= 0) nb_ex = 1;

// enlever les '\n'
l.isbn[strcspn(l.isbn, "\n")] = 0;
l.titre[strcspn(l.titre, "\n")] = 0;
l.auteur[strcspn(l.auteur, "\n")] = 0;
l.categorie[strcspn(l.categorie, "\n")] = 0;

l.nb_exemplaires_total = nb_ex;
l.nb_exemplaires_disponibles = nb_ex;
l.dispo = LIVRE_DISPONIBLE;

StatutLivre st = ajouterLivre(&b, &l);
if (st == STATUT_OK) {
printf(" Livre ajoute !\n");
} else if (st == STATUT_ENTREE_INVALIDE) {
printf(" Erreur : entree invalide (ISBN vide ou annee incorrecte).\n");
} else {
printf(" Erreur ajout livre (code %d).\n", st);
}

printf("\nAppuyez sur Entree pour continuer...");
fgets(buffer, sizeof(buffer), stdin);
break;
}

/* ======================
2. SUPPRIMER UN LIVRE
====================== */
case 2:
printf("ISBN du livre a supprimer : ");
fgets(isbn, sizeof(isbn), stdin);
isbn[strcspn(isbn, "\n")] = 0;

if (supprimerLivre(&b, isbn) == STATUT_OK)
printf(" Livre supprime !\n");
else
printf(" Livre non trouve.\n");
break;

/* ======================
3. MODIFIER UN LIVRE
====================== */
case 3:
printf("ISBN du livre a modifier : ");
fgets(isbn, sizeof(isbn), stdin);
isbn[strcspn(isbn, "\n")] = 0;

// Réinitialiser la structure UpdateLivre
memset(&u, 0, sizeof(u));

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

printf("Nouvelle annee (0 si pas changer) : ");
scanf("%d", &annee);
getchar();
if (annee != 0) {
u.mask |= UPD_ANNEE;
u.new_annee = annee;
}

printf("Nouvelle categorie (laisser vide si rien) : ");
fgets(buffer, sizeof(buffer), stdin);
if (buffer[0] != '\n') {
buffer[strcspn(buffer, "\n")] = 0;
u.mask |= UPD_CATEGORIE;
strcpy(u.new_categorie, buffer);
}

if (modifierLivre(&b, isbn, &u) == STATUT_OK)
printf(" Livre modifie.\n");
else
printf(" Livre non trouve.\n");
break;

/* ============
RECHERCHE
============ */
case 4:
printf("Titre : ");
fgets(buffer, sizeof(buffer), stdin);
buffer[strcspn(buffer, "\n")] = 0;
rechercherLivreParTitre(&b, buffer);
break;

case 5:
printf("Auteur : ");
fgets(buffer, sizeof(buffer), stdin);
buffer[strcspn(buffer, "\n")] = 0;
rechercherLivreParAuteur(&b, buffer);
break;

case 6:
printf("ISBN : ");
fgets(buffer, sizeof(buffer), stdin);
buffer[strcspn(buffer, "\n")] = 0;
rechercherLivreParISBN(&b, buffer);
break;

case 7:
printf("Categorie : ");
fgets(buffer, sizeof(buffer), stdin);
buffer[strcspn(buffer, "\n")] = 0;
rechercherLivreParCategorie(&b, buffer);
break;

/* ===================
AFFICHAGE / TRI
=================== */
case 8:
        afficherTousLesLivresTable(&b);
break;

case 9:
printf("Annee : ");
scanf("%d", &annee);
getchar();
afficherLivresDuneAnnee(&b, annee);
break;

/* ========================================
10. AFFICHER DISPONIBILITE / EXEMPLAIRES
======================================== */
case 10: {
printf("ISBN : ");
fgets(isbn, sizeof(isbn), stdin);
isbn[strcspn(isbn, "\n")] = 0;

// Affichage existant (ton ancienne fonction)
afficherDisponibiliteLivre(&b, isbn);

// Ajout : affichage du nombre d'exemplaires
int dispo_ex = getNbExemplairesDisponibles(&b, isbn);
if (dispo_ex >= 0) {
printf(" Nombre d'exemplaires disponibles : %d\n", dispo_ex);
} else {
printf(" Livre introuvable.\n");
}
break;
}

/* ================================
11. MODIFIER LA DISPONIBILITE
================================ */
case 11: {
int etat;
printf("ISBN : ");
fgets(isbn, sizeof(isbn), stdin);
isbn[strcspn(isbn, "\n")] = 0;

printf("Nouvel etat (0=dispo 1=emprunte 2=reserve 3=perdu 4=HS) : ");
scanf("%d", &etat);
getchar();

setDisponibiliteLivre(&b, isbn, (Disponibilite)etat);
break;
}

/* ===========================
12. EMPRUNTER UN EXEMPLAIRE
=========================== */
case 12: {
printf("ISBN du livre a emprunter : ");
fgets(isbn, sizeof(isbn), stdin);
isbn[strcspn(isbn, "\n")] = 0;

int dispo_ex = getNbExemplairesDisponibles(&b, isbn);
if (dispo_ex < 0) {
printf(" Livre introuvable.\n");
break;
}

printf(" Il reste %d exemplaire(s) disponible(s) pour ce livre.\n", dispo_ex);
if (dispo_ex == 0) {
printf(" Aucun exemplaire disponible : emprunt impossible.\n");
break;
}

printf(" Confirmer l'emprunt ? (o/n) : ");
char rep = (char)getchar();
getchar(); // consomme le '\n'

if (rep != 'o' && rep != 'O') {
printf(" Emprunt annule.\n");
break;
}

StatutLivre st = emprunterExemplaire(&b, isbn);
if (st == STATUT_OK) {
printf(" Emprunt enregistre.\n");
} else if (st == STATUT_PLUS_DISPONIBLE) {
printf(" Plus aucun exemplaire disponible.\n");
} else if (st == STATUT_LIVRE_INEXISTANT) {
printf(" Livre introuvable.\n");
} else {
printf(" Erreur lors de l'emprunt (code %d).\n", st);
}
break;
}

/* ==========================
13. RETOURNER UN EXEMPLAIRE
========================== */
case 13: {
printf("ISBN du livre a retourner : ");
fgets(isbn, sizeof(isbn), stdin);
isbn[strcspn(isbn, "\n")] = 0;

StatutLivre st = retournerExemplaire(&b, isbn);
if (st == STATUT_OK) {
printf(" Retour enregistre.\n");
} else if (st == STATUT_LIVRE_INEXISTANT) {
printf(" Livre introuvable.\n");
} else {
printf(" Erreur lors du retour (code %d).\n", st);
}
break;
}

/* =================
STATISTIQUES
================= */
case 14: {
    afficherStatistiquesBibliotheque(&b);
    break;
}


case 15: {
    if (genererRapportStatistiques(&b, "rapport_stats.txt") == STATUT_OK)
        printf(" Rapport exporte dans rapport_stats.txt\n");
    else
        printf(" Erreur lors de la generation du rapport.\n");
    break;
}

/* =========
QUITTER
========= */
case 0:
printf(" Au revoir !\n");
        sauvegarderBibliothequeTable(&b, "livres_table.txt");
printf(" Donnees sauvegardees.\n");
break;

default:
printf(" Choix invalide.\n");
break;
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