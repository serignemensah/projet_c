cmake_minimum_required(VERSION 3.29)
project(ProjetInfo C)

set(CMAKE_C_STANDARD 11)

add_executable(ProjetInfo main.c
        partie_livres/bibliotheque.c
        partie_livres/bibliotheque.h
        partie_livres/recherche.c
        partie_livres/recherche.h
        partie_livres/test.c
        partie_livres/test.h
        partie_livres/statistiques.c
        partie_livres/statistiques.h
        fonction/partie_A/utilisateurs.h
        fonction/partie_A/utilisateurs.c
        fonction/partie_B/recherche_utilisateur.c
        fonction/partie_B/recherche_utilisateur.h
)
