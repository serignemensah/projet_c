cmake_minimum_required(VERSION 4.0)
project(projet_c C)

set(CMAKE_C_STANDARD 11)

include_directories(partie_A partie_B)

add_executable(projet_c
        main.c
        partie_A/utilisateurs.c
        partie_B/recherche_utilisateur.c)
