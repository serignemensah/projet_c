#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../partie_A/utilisateurs.h"
#include "recherche_utilisateur.h"

// Conversion en minuscules
void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

// Suppression des espaces superflus et du \n
void trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

// Recherche par ID
Utilisateur* rechercherUtilisateurParID(BaseUtilisateurs *base, int id) {
    for (int i = 0; i < base->nbUtilisateurs; i++) {
        if (base->utilisateurs[i].id == id)
            return &base->utilisateurs[i];
    }
    return NULL;
}

// Recherche par email exact
Utilisateur* rechercherUtilisateurParEmail(BaseUtilisateurs *base, const char *email) {
    for (int i = 0; i < base->nbUtilisateurs; i++) {
        if (strcmp(base->utilisateurs[i].email, email) == 0)
            return &base->utilisateurs[i];
    }
    return NULL;
}

// Recherche par sous-chaîne de nom
void rechercherUtilisateurParNom(BaseUtilisateurs *base, const char *nom) {
    char nomRecherche[100];
    strcpy(nomRecherche, nom);
    toLowerCase(nomRecherche);
    int trouve = 0;

    for (int i = 0; i < base->nbUtilisateurs; i++) {
        char nomActuel[100];
        strcpy(nomActuel, base->utilisateurs[i].nom);
        toLowerCase(nomActuel);
        if (strstr(nomActuel, nomRecherche)) {
            printf("ID: %d | %s %s | %s\n",
                   base->utilisateurs[i].id,
                   base->utilisateurs[i].prenom,
                   base->utilisateurs[i].nom,
                   base->utilisateurs[i].email);
            trouve = 1;
        }
    }
    if (!trouve)
        printf("Aucun utilisateur trouvé pour le nom : %s\n", nom);
}

// Affichage de tous les utilisateurs
void afficherUtilisateurs(BaseUtilisateurs *base) {
    if (base->nbUtilisateurs == 0) {
        printf("Aucun utilisateur enregistré.\n");
        return;
    }
    printf("\n%-5s | %-15s | %-15s | %-25s\n", "ID", "Nom", "Prénom", "E-mail");
    printf("-----------------------------------------------------------------------\n");
    for (int i = 0; i < base->nbUtilisateurs; i++) {
        printf("%-5d | %-15s | %-15s | %-25s\n",
               base->utilisateurs[i].id,
               base->utilisateurs[i].nom,
               base->utilisateurs[i].prenom,
               base->utilisateurs[i].email);
    }
}

// Tri par nom
int comparerParNom(const void *a, const void *b) {
    const Utilisateur *u1 = (const Utilisateur *)a;
    const Utilisateur *u2 = (const Utilisateur *)b;

    int cmp = strcmp(u1->nom, u2->nom);
    if (cmp == 0)
        cmp = strcmp(u1->prenom, u2->prenom);
    return cmp;
}

// Tri par ID
int comparerParID(const void *a, const void *b) {
    const Utilisateur *u1 = (const Utilisateur *)a;
    const Utilisateur *u2 = (const Utilisateur *)b;
    return u1->id - u2->id;
}

void afficherUtilisateursTriesParNom(BaseUtilisateurs *base) {
    if (base->nbUtilisateurs == 0) {
        printf("Aucun utilisateur à trier.\n");
        return;
    }
    qsort(base->utilisateurs, base->nbUtilisateurs, sizeof(Utilisateur), comparerParNom);
    afficherUtilisateurs(base);
}

void afficherUtilisateursTriesParID(BaseUtilisateurs *base) {
    if (base->nbUtilisateurs == 0) {
        printf("Aucun utilisateur à trier.\n");
        return;
    }
    qsort(base->utilisateurs, base->nbUtilisateurs, sizeof(Utilisateur), comparerParID);
    afficherUtilisateurs(base);
}

// Menu principal
void menu_recherche(BaseUtilisateurs *base) {
    int choix;
    do {
        printf("\n===== MENU GESTION UTILISATEURS =====\n");
        printf("1. Ajouter un utilisateur\n");
        printf("2. Modifier un utilisateur\n");
        printf("3. Supprimer un utilisateur\n");
        printf("4. Rechercher un utilisateur par nom\n");
        printf("5. Lister tous les utilisateurs\n");
        printf("6. Lister triés par nom\n");
        printf("7. Lister triés par ID\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        getchar(); // éviter le \n

        if (choix == 1) {
            Utilisateur u;
            printf("ID : "); scanf("%d", &u.id); getchar();
            printf("Nom : "); fgets(u.nom, sizeof(u.nom), stdin); trim(u.nom);
            printf("Prénom : "); fgets(u.prenom, sizeof(u.prenom), stdin); trim(u.prenom);
            printf("Email : "); fgets(u.email, sizeof(u.email), stdin); trim(u.email);

            StatutUtilisateur status = ajouterUtilisateur(base, u);
            if (status == OK) printf("✅ Utilisateur ajouté.\n");
            else printf("❌ Erreur (code %d).\n", status);
        }

        else if (choix == 2) {
            int id;
            printf("ID à modifier : "); scanf("%d", &id); getchar();
            Utilisateur *u = rechercherUtilisateurParID(base, id);
            if (!u) { printf("Utilisateur introuvable.\n"); continue; }

            UpdateUtilisateur upd = {id, "", "", ""};

            printf("Nouveau nom (%s) : ", u->nom);
            fgets(upd.nom, sizeof(upd.nom), stdin); trim(upd.nom);

            printf("Nouveau prénom (%s) : ", u->prenom);
            fgets(upd.prenom, sizeof(upd.prenom), stdin); trim(upd.prenom);

            printf("Nouvel email (%s) : ", u->email);
            fgets(upd.email, sizeof(upd.email), stdin); trim(upd.email);

            StatutUtilisateur s = modifierUtilisateur(base, upd);
            if (s == OK) printf(" Modifié avec succès.\n");
            else printf(" Erreur (code %d).\n", s);
        }

        else if (choix == 3) {
            int id;
            printf("ID à supprimer : "); scanf("%d", &id); getchar();
            StatutUtilisateur s = supprimerUtilisateurParID(base, id);
            if (s == OK) printf(" Supprimé.\n");
            else printf(" Erreur (code %d).\n", s);
        }

        else if (choix == 4) {
            char nom[100];
            printf("Nom à rechercher : ");
            fgets(nom, sizeof(nom), stdin);
            trim(nom);
            rechercherUtilisateurParNom(base, nom);
        }

        else if (choix == 5) afficherUtilisateurs(base);
        else if (choix == 6) afficherUtilisateursTriesParNom(base);
        else if (choix == 7) afficherUtilisateursTriesParID(base);

    } while (choix != 0);
    printf("👋 Fin du programme.\n");
}
