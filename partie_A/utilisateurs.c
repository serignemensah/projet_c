#include <stdio.h>
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
