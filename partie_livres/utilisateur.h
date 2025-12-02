//
// Created by marie on 27/11/2025.
//

#ifndef UTILISATEUR_H
#define UTILISATEUR_H
#define MAX_UTILISATEURS 100

#define ADMIN_PASSWORD "2002"

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
    OK,
    UTILISATEUR_EXISTE,
    UTILISATEUR_INEXISTANT,
    BASE_PLEINE
} StatutUtilisateur;
typedef enum {
    ROLE_ADMIN = 1,
    ROLE_UTILISATEUR = 2,

} UserRole;

int Admin(Utilisateur *u);

// Prototypes CRUD
StatutUtilisateur ajouterUtilisateur(BaseUtilisateurs *base, Utilisateur u);
StatutUtilisateur modifierUtilisateur(BaseUtilisateurs *base, UpdateUtilisateur upd);
StatutUtilisateur supprimerUtilisateurParID(BaseUtilisateurs *base, int id);

void toLowerCase(char *str);
void trim(char *str);
Utilisateur* rechercherUtilisateurParID(BaseUtilisateurs *base, int id);
Utilisateur* rechercherUtilisateurParEmail(BaseUtilisateurs *base, const char *email);
void rechercherUtilisateurParNom(BaseUtilisateurs *base, const char *nom);
void afficherUtilisateurs(BaseUtilisateurs *base);
void afficherUtilisateursTriesParNom(BaseUtilisateurs *base);
void afficherUtilisateursTriesParID(BaseUtilisateurs *base);
void menu_recherche(BaseUtilisateurs *base, int role);
// Persistance
StatutUtilisateur sauvegarderUtilisateurs(BaseUtilisateurs *base, const char *filename);
StatutUtilisateur chargerUtilisateurs(BaseUtilisateurs *base, const char *filename);
int existeUtilisateur(BaseUtilisateurs *base, int id);


#endif //UTILISATEUR_H