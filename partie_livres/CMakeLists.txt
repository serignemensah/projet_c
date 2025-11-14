cmake_minimum_required(VERSION 3.29)
project(ProjetInfo C)

set(CMAKE_C_STANDARD 11)

add_executable(ProjetInfo main.c
        bibliothèque.c
        bibliothèque.h
        recherche.c
        recherche.h
        test.c
        test.h
        statistiques.c
        statistiques.h
)
