#ifndef EMPRUNT_H
#define EMPRUNT_H

#include "recherche.h"    // Bibliotheque, Livre, ISBN_LEN...
#include "utilisateur.h"  // BaseUtilisateurs

#define MAX_EMPRUNTS          100
#define MAX_PRETS_PAR_USER    5      // plafond théorique
#define DELAI_RETARD_JOURS    15
#define AMENDE_PAR_JOUR       1.0

/* Date simple jj/mm/aaaa */
typedef struct {
    int j, m, a;
} Date;

/* Emprunt : historique complet */
typedef struct {
    int    idEmprunt;                 /* identifiant interne (unique) */
    int    idUtilisateur;             /* ID utilisateur (int)         */
    char   isbn[ISBN_LEN];            /* ISBN du livre emprunté       */
    Date   dateEmprunt;               /* date d'emprunt               */
    Date   dateRetour;                /* date de retour (si rendu)    */
    int    actif;                     /* 1 = en cours, 0 = terminé    */
    double amende;                    /* amende si retard             */
    int    numExemplaire;             /* (optionnel)                  */
} Emprunt;

/* Base des emprunts (historique complet) */
typedef struct {
    Emprunt tab[MAX_EMPRUNTS];
    int     nbEmprunts;
    int     nextId;   /* pour attribuer des idEmprunt croissants */
} BaseEmprunts;

/* Statuts possibles */
typedef enum {
    EMPRUNT_OK = 0,
    EMPRUNT_EXISTE,
    EMPRUNT_INEXISTANT,
    BASE_EMPRUNT_PLEINE,
    EMPRUNT_PLUS_DISPONIBLE,
    EMPRUNT_UTILISATEUR_INEXISTANT,
    EMPRUNT_QUOTA_ATTEINT  /* quand pénalités réduisent le quota à 0 */
} StatutEmprunt;

/* --- dates et utilitaires --- */
void initDateAujourdHui(Date *d);
int  differenceJours(Date debut, Date fin);

/* demande une date à l'utilisateur, avec un message personnalisé */
Date lireDateDepuisClavier(const char *message);

/* compte emprunts actifs pour un utilisateur */
int compterEmpruntsActifs(BaseEmprunts *be, int userId);

/* compte le nombre de pénalités (emprunts rendus en retard) d'un utilisateur */
int compterPenalites(BaseEmprunts *be, int userId);

/* --- logique d'emprunt / retour --- */
StatutEmprunt emprunterLivre(BaseEmprunts *be,
                             Bibliotheque *b,
                             BaseUtilisateurs *bu,
                             const char *isbn,
                             int userId);

StatutEmprunt retournerLivre(BaseEmprunts *be,
                             Bibliotheque *b,
                             int empruntId);

/* --- affichage / stats --- */
void listerEmpruntsActifs(BaseEmprunts *be);
void detecterRetards(BaseEmprunts *be);

StatutEmprunt sauvegarderEmprunts(BaseEmprunts *be, const char *filename);
StatutEmprunt rechargerEmprunts(BaseEmprunts *be, const char *filename);

void statistiquesEmprunts(BaseEmprunts *be,
                          Bibliotheque *b,
                          BaseUtilisateurs *bu);

/* --- menu console --- */
void afficherMenuEmprunt(Bibliotheque *b,
                         BaseUtilisateurs *bu,
                         BaseEmprunts *be);

/* --- sauvegarde / chargement global (raccourcis) --- */
void sauvegarderToutesDonnees(Bibliotheque *b,
                              BaseUtilisateurs *bu,
                              BaseEmprunts *be);

void chargerToutesDonnees(Bibliotheque *b,
                          BaseUtilisateurs *bu,
                          BaseEmprunts *be);

#endif /* EMPRUNT_H */