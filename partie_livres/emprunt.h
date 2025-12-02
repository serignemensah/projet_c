#ifndef EMPRUNT_H
#define EMPRUNT_H

#include "recherche.h"
#include "utilisateur.h"


#define MAX_EMPRUNTS 100
#define MAX_PRETS_PAR_USER 3
#define DELAI_RETARD_JOURS 15
#define AMENDE_PAR_JOUR 1.0

/* Date struct (jour/mois/annee) */
typedef struct {
    int j, m, a;
} Date;

/* Emprunt : stocke ISBN du livre et ID (int) de l'utilisateur */
typedef struct {
    int idEmprunt;                  /* identifiant interne (unique) */
    int idUtilisateur;              /* identifiant utilisateur (int) */
    char isbn[ISBN_LEN];            /* ISBN du livre emprunté */
    Date dateEmprunt;               /* date d'emprunt */
    Date dateRetour;                /* date de retour (si rendu) */
    int actif;                      /* 1 = en cours, 0 = terminé */
    double amende;                  /* amende calculée au retour */
    int numExemplaire;              /* numéro d'exemplaire (optionnel) */
} Emprunt;

typedef struct {
    Emprunt tab[MAX_EMPRUNTS];
    int nbEmprunts;
    int nextId; /* pour attribuer des idEmprunt croissants */
} BaseEmprunts;

/* Statuts */
typedef enum {
    EMPRUNT_OK = 0,
    EMPRUNT_EXISTE,
    EMPRUNT_INEXISTANT,
    BASE_EMPRUNT_PLEINE,
    EMPRUNT_PLUS_DISPONIBLE,
    EMPRUNT_UTILISATEUR_INEXISTANT
} StatutEmprunt;

/* Prototypes publiques */
void initDateAujourdHui(Date *d);
int differenceJours(Date d1, Date d2);

int compterEmpruntsActifs(BaseEmprunts *be, int userId);

StatutEmprunt emprunterLivre(BaseEmprunts *be, Bibliotheque *b, BaseUtilisateurs *bu, const char *isbn, int userId);
StatutEmprunt retournerLivre(BaseEmprunts *be, Bibliotheque *b, int empruntId);

void listerEmpruntsActifs(BaseEmprunts *be);
void detecterRetards(BaseEmprunts *be);

StatutEmprunt sauvegarderEmprunts(BaseEmprunts *be, const char *filename);
StatutEmprunt rechargerEmprunts(BaseEmprunts *be, const char *filename);

void sauvegarderEmpruntStat(BaseEmprunts *be, Bibliotheque *b); /* met à jour compteurs livres */
void statistiquesEmprunts(BaseEmprunts *be, Bibliotheque *b, BaseUtilisateurs *bu);

/* Menu emprunt (console) */
void afficherMenuEmprunt(Bibliotheque *b, BaseUtilisateurs *bu, BaseEmprunts *be);

/* fonctions globales sauvegarde/chargement (fasseuses) */
void sauvegarderToutesDonnees(Bibliotheque *b, BaseUtilisateurs *bu, BaseEmprunts *be);
void chargerToutesDonnees(Bibliotheque *b, BaseUtilisateurs *bu, BaseEmprunts *be);

#endif /* EMPRUNT_H */
