//
// Created by marie on 03/11/2025.
//

#ifndef STATISTIQUES_H
#define STATISTIQUES_H
#include "bibliotheque.h"
int compterTotalLivres(const Bibliotheque *b);
int compterLivresParStatut(const Bibliotheque *b, Disponibilite d);
int compterLivresDisponibles(const Bibliotheque *b);
Livre* trouverLivrePlusAncien(const Bibliotheque *b);
Livre* trouverLivrePlusRecent(const Bibliotheque *b);
void afficherStatistiquesBibliotheque(const Bibliotheque *b);
int compterLivresParCategorie(const Bibliotheque *b, const char *categorie);
void afficherStatsParCategorie(const Bibliotheque *b);
StatutLivre genererRapportStatistiques(const Bibliotheque *b, const char *filename);

#endif //STATISTIQUES_H
