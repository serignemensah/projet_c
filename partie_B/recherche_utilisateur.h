#ifndef RECHERCHE_UTILISATEUR_H
#define RECHERCHE_UTILISATEUR_H

#include "../partie_A/utilisateurs.h"

void toLowerCase(char *str);
void trim(char *str);
Utilisateur* rechercherUtilisateurParID(BaseUtilisateurs *base, int id);
Utilisateur* rechercherUtilisateurParEmail(BaseUtilisateurs *base, const char *email);
void rechercherUtilisateurParNom(BaseUtilisateurs *base, const char *nom);
void afficherUtilisateurs(BaseUtilisateurs *base);
void afficherUtilisateursTriesParNom(BaseUtilisateurs *base);
void afficherUtilisateursTriesParID(BaseUtilisateurs *base);
void menu(BaseUtilisateurs *base);
// Persistance
StatutUtilisateur sauvegarderUtilisateurs(BaseUtilisateurs *base, const char *filename);
StatutUtilisateur chargerUtilisateurs(BaseUtilisateurs *base, const char *filename);

#endif // RECHERCHE_UTILISATEUR_H
