
#ifndef PROJETINFO_EMPRUNT_H
#define PROJETINFO_EMPRUNT_H
#define MAX_EMPRUNTS 500

// Structure représentant un emprunt
typedef struct {
    int idEmprunt;
    int idUtilisateur;
    int idLivre;
    char dateEmprunt[20];
    char dateRetour[20];
    int rendu; // 0 = non rendu, 1 = rendu
} Emprunt;
typedef struct {
    int idEmprunt;
    int idUtilisateur;
    int idLivre;
    char dateEmprunt[20];
    char dateRetour[20];
    int rendu; // 0 = non rendu, 1 = rendu
}UpdateEmprunt;


// Structure représentant la base d'emprunts
typedef struct {
    Emprunt emprunts[MAX_EMPRUNTS];
    int nbEmprunts;
} BaseEmprunts;

// Codes de statut
typedef enum {
    EMPRUNT_EXISTE ,
    EMPRUNT_INEXISTANT ,
    BASE_EMPRUNT_PLEINE
} StatutEmprunt;

// Déclarations des fonctions
void afficherMenuEmprunt(BaseEmprunts *base_emprunts);

int existeEmprunt(BaseEmprunts *base_emprunts, int id);

StatutEmprunt ajouterEmprunt(BaseEmprunts *base_emprunts, Emprunt e);
StatutEmprunt modifierEmprunt(BaseEmprunts *base_emprunts, Emprunt upd);
StatutEmprunt supprimerEmpruntParID(BaseEmprunts *base_emprunts, int id);

Emprunt* rechercherEmpruntParID(BaseEmprunts *base_emprunts, int id);
StatutEmprunt sauvegarderEmprunts(BaseEmprunts *base_emprunts, const char *filename);


void afficherMenuEmprunt(BaseEmprunts *base_emprunts);


#endif //PROJETINFO_EMPRUNT_H