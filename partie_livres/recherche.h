//
// Created by marie on 03/11/2025.
//

#ifndef RECHERCHE_H
#define RECHERCHE_H
#include "bibliotheque.h"

void rechercherLivreParTitre(const Bibliotheque *b, const char *titre);
void rechercherLivreParAuteur(const Bibliotheque *b, const char *auteur);
void rechercherLivreParISBN(const Bibliotheque *b, const char *isbn);
void rechercherLivreParCategorie(const Bibliotheque *b, const char *categorie);
void afficherLivresParTitre(const Bibliotheque *b);
void afficherLivresParAnnee(const Bibliotheque *b);
void afficherDisponibiliteLivre(const Bibliotheque *b, const char *isbn);
StatutLivre setDisponibiliteLivre(Bibliotheque *b, const char *isbn, Disponibilite d);
void afficherLivresDuneAnnee(const Bibliotheque *b, int annee);


#endif //RECHERCHE_H
