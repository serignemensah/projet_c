#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilisateurs.h"

// Vérifie si un utilisateur existe déjà (par ID)
int existeUtilisateur(BaseUtilisateurs *base, int id) {
    for (int i = 0; i < base->nbUtilisateurs; i++) {
        if (base->utilisateurs[i].id == id)
            return 1;
    }
    return 0;
}

// Ajouter
StatutUtilisateur ajouterUtilisateur(BaseUtilisateurs *base, Utilisateur u) {
    if (base->nbUtilisateurs >= MAX_UTILISATEURS)
        return BASE_PLEINE;
    if (existeUtilisateur(base, u.id))
        return UTILISATEUR_EXISTE;

    base->utilisateurs[base->nbUtilisateurs++] = u;
    return OK;
}

// Modifier
StatutUtilisateur modifierUtilisateur(BaseUtilisateurs *base, UpdateUtilisateur upd) {
    for (int i = 0; i < base->nbUtilisateurs; i++) {
        if (base->utilisateurs[i].id == upd.id) {
            if (strlen(upd.nom) > 0)
                strcpy(base->utilisateurs[i].nom, upd.nom);
            if (strlen(upd.prenom) > 0)
                strcpy(base->utilisateurs[i].prenom, upd.prenom);
            if (strlen(upd.email) > 0)
                strcpy(base->utilisateurs[i].email, upd.email);
            return OK;
        }
    }
    return UTILISATEUR_INEXISTANT;
}

// Supprimer
StatutUtilisateur supprimerUtilisateurParID(BaseUtilisateurs *base, int id) {
    for (int i = 0; i < base->nbUtilisateurs; i++) {
        if (base->utilisateurs[i].id == id) {
            for (int j = i; j < base->nbUtilisateurs - 1; j++)
                base->utilisateurs[j] = base->utilisateurs[j + 1];
            base->nbUtilisateurs--;
            return OK;
        }
    }
    return UTILISATEUR_INEXISTANT;
}

StatutUtilisateur sauvegarderUtilisateurs(BaseUtilisateurs *base, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return BASE_PLEINE; // erreur ouverture fichier

    for (int i = 0; i < base->nbUtilisateurs; i++) {
        fprintf(f, "%d,%s,%s,%s\n",
                base->utilisateurs[i].id,
                base->utilisateurs[i].nom,
                base->utilisateurs[i].prenom,
                base->utilisateurs[i].email);
    }

    fclose(f);
    return OK;
}

StatutUtilisateur chargerUtilisateurs(BaseUtilisateurs *base, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return BASE_PLEINE; // fichier inexistant

    base->nbUtilisateurs = 0;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        if (base->nbUtilisateurs >= MAX_UTILISATEURS) break;

        Utilisateur u;
        char *token = strtok(line, ",");
        if (!token) continue;
        u.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(u.nom, token, sizeof(u.nom));
        u.nom[strcspn(u.nom, "\n")] = 0;

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(u.prenom, token, sizeof(u.prenom));
        u.prenom[strcspn(u.prenom, "\n")] = 0;

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(u.email, token, sizeof(u.email));
        u.email[strcspn(u.email, "\n")] = 0;

        base->utilisateurs[base->nbUtilisateurs++] = u;
    }

    fclose(f);
    return OK;
}
