#ifndef UTILISATEURS_H
#define UTILISATEURS_H

#define MAX_UTILISATEURS 100

typedef struct {
    int id;
    char nom[100];
    char prenom[100];
    char email[100];
} Utilisateur;

typedef struct {
    Utilisateur utilisateurs[MAX_UTILISATEURS];
    int nbUtilisateurs;
} BaseUtilisateurs;

typedef struct {
    int id;
    char nom[100];
    char prenom[100];
    char email[100];
} UpdateUtilisateur;

typedef enum {
    OK = 0,
    UTILISATEUR_EXISTE,
    UTILISATEUR_INEXISTANT,
    BASE_PLEINE
} StatutUtilisateur;

// Prototypes CRUD
StatutUtilisateur ajouterUtilisateur(BaseUtilisateurs *base, Utilisateur u);
StatutUtilisateur modifierUtilisateur(BaseUtilisateurs *base, UpdateUtilisateur upd);
StatutUtilisateur supprimerUtilisateurParID(BaseUtilisateurs *base, int id);

#endif
ILISATEURS_H