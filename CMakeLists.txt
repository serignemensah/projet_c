cmake_minimum_required(VERSION 3.29)
project(ProjetInfo C)

set(CMAKE_C_STANDARD 11)

add_executable(ProjetInfo main.c
        partie_livres/bibliothèque.c
        partie_livres/bibliothèque.h
        partie_livres/recherche.c
        partie_livres/recherche.h
        partie_livres/test.c
        partie_livres/test.h
        partie_livres/statistiques.c
        partie_livres/statistiques.h
        partie_A/utilisateurs.h
        partie_A/utilisateurs.c
        partie_B/recherche_utilisateur.c
        partie_B/recherche_utilisateur.h
)
