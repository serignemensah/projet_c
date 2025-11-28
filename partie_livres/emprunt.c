#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "emprunt.h"
#include "bibliotheque.h"
#include "utilisateur.h"

/* ---------- utilitaires ---------- */

static time_t dateToTimeT(Date d) {
    struct tm t = {0};
    t.tm_mday = d.j;
    t.tm_mon  = d.m - 1;
    t.tm_year = d.a - 1900;
    t.tm_isdst = -1;
    return mktime(&t);
}

void initDateAujourdHui(Date *d) {
    time_t now = time(NULL);
    struct tm *tmv = localtime(&now);
    d->j = tmv->tm_mday;
    d->m = tmv->tm_mon + 1;
    d->a = tmv->tm_year + 1900;
}

int differenceJours(Date d1, Date d2) {
    time_t t1 = dateToTimeT(d1);
    time_t t2 = dateToTimeT(d2);
    double sec = difftime(t2, t1);
    return (int)(sec / 86400.0);
}

/* Cherche un livre par ISBN dans Bibliotheque -> renvoie index ou -1 */
static int indexLivreParISBN(const Bibliotheque *b, const char *isbn) {
    if (!b || !isbn) return -1;
    for (size_t i = 0; i < b->nb; ++i)
        if (strcmp(b->livres[i].isbn, isbn) == 0) return (int)i;
    return -1;
}

/* Cherche utilisateur par id (int), renvoie 1 si existe, 0 sinon */
static int utilisateurExiste(BaseUtilisateurs *bu, int userId) {
    if (!bu) return 0;
    for (int i = 0; i < bu->nbUtilisateurs; ++i)
        if (bu->utilisateurs[i].id == userId) return 1;
    return 0;
}

/* Compte emprunts actifs pour userId */
int compterEmpruntsActifs(BaseEmprunts *be, int userId) {
    if (!be) return 0;
    int c = 0;
    for (int i = 0; i < be->nbEmprunts; ++i)
        if (be->tab[i].actif && be->tab[i].idUtilisateur == userId) ++c;
    return c;
}

/* ---------- emprunt / retour ---------- */

/* Emprunter : vérifie existence livre/utilisateur, disponibilité, quota utilisateur */
StatutEmprunt emprunterLivre(BaseEmprunts *be, Bibliotheque *b, BaseUtilisateurs *bu, const char *isbn, int userId) {
    if (!be || !b || !bu || !isbn) return EMPRUNT_INEXISTANT;

    int idxL = indexLivreParISBN(b, isbn);
    if (idxL < 0) return EMPRUNT_INEXISTANT;

    /* vérifier utilisateur */
    if (!utilisateurExiste(bu, userId)) return EMPRUNT_UTILISATEUR_INEXISTANT;

    Livre *L = &b->livres[idxL];

    if (L->nb_exemplaires_disponibles <= 0) return EMPRUNT_PLUS_DISPONIBLE;

    if (compterEmpruntsActifs(be, userId) >= MAX_PRETS_PAR_USER) return BASE_EMPRUNT_PLEINE;

    if (be->nbEmprunts >= MAX_EMPRUNTS) return BASE_EMPRUNT_PLEINE;

    /* créer emprunt */
    Emprunt e;
    e.idEmprunt = be->nextId++;
    e.idUtilisateur = userId;
    strncpy(e.isbn, isbn, ISBN_LEN-1); e.isbn[ISBN_LEN-1] = '\0';
    initDateAujourdHui(&e.dateEmprunt);
    e.dateRetour.j = e.dateRetour.m = e.dateRetour.a = 0;
    e.actif = 1;
    e.amende = 0.0;
    e.numExemplaire = 0; /* si tu veux numéro précis, il faudrait un mapping d'exemplaires */

    be->tab[be->nbEmprunts++] = e;

    /* mettre à jour disponibilité du livre */
    L->nb_exemplaires_disponibles--;
    if (L->nb_exemplaires_disponibles == 0) L->dispo = LIVRE_EMPRUNTE;
    L->nb_Emprunts++; /* compteur statistique */

    return EMPRUNT_OK;
}

/* Retourner : on identifie l'emprunt par idEmprunt (int). */
StatutEmprunt retournerLivre(BaseEmprunts *be, Bibliotheque *b, int empruntId) {
    if (!be || !b) return EMPRUNT_INEXISTANT;

    int pos = -1;
    for (int i = 0; i < be->nbEmprunts; ++i)
        if (be->tab[i].idEmprunt == empruntId && be->tab[i].actif) { pos = i; break; }

    if (pos == -1) return EMPRUNT_INEXISTANT;

    Emprunt *e = &be->tab[pos];
    Date dr; initDateAujourdHui(&dr);
    e->dateRetour = dr;
    e->actif = 0;

    /* Mise à jour livre : retrouver index par ISBN et incrémenter dispo */
    int idxL = indexLivreParISBN(b, e->isbn);
    if (idxL >= 0) {
        Livre *L = &b->livres[idxL];
        if (L->nb_exemplaires_disponibles < L->nb_exemplaires_total)
            L->nb_exemplaires_disponibles++;
        if (L->nb_exemplaires_disponibles > 0) L->dispo = LIVRE_DISPONIBLE;
    }

    /* calcul amende si retard */
    int duree = differenceJours(e->dateEmprunt, dr);
    if (duree > DELAI_RETARD_JOURS) {
        int joursRetard = duree - DELAI_RETARD_JOURS;
        e->amende = (double)joursRetard * AMENDE_PAR_JOUR;
    } else {
        e->amende = 0.0;
    }

    return EMPRUNT_OK;
}

/* ---------- affichage / retards / statistiques ---------- */

void listerEmpruntsActifs(BaseEmprunts *be) {
    if (!be) { printf("Aucune base d'emprunts.\n"); return; }
    printf("\n--- Emprunts actifs (%d) ---\n", be->nbEmprunts);
    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        if (e->actif) {
            printf("ID:%d | ISBN:%s | UserID:%d | Date:%02d/%02d/%04d\n",
                   e->idEmprunt, e->isbn, e->idUtilisateur,
                   e->dateEmprunt.j, e->dateEmprunt.m, e->dateEmprunt.a);
        }
    }
}

void detecterRetards(BaseEmprunts *be) {
    if (!be) { printf("Aucune base d'emprunts.\n"); return; }
    Date aujourd; initDateAujourdHui(&aujourd);
    printf("\n--- Retards > %d jours ---\n", DELAI_RETARD_JOURS);
    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        if (e->actif) {
            int duree = differenceJours(e->dateEmprunt, aujourd);
            if (duree > DELAI_RETARD_JOURS) {
                printf("ID:%d | ISBN:%s | User:%d | emprunte il y a %d jours\n",
                       e->idEmprunt, e->isbn, e->idUtilisateur, duree);
            }
        }
    }
}

/* Met à jour les compteurs des livres (déjà fait lors d'emprunt), mais on peut recalculer */
void sauvegarderEmpruntStat(BaseEmprunts *be, Bibliotheque *b) {
    (void)be;
    (void)b;
    /* Ici on laisse vide : les compteurs nb_Emprunts sont mis à jour lors d'emprunterLivre. */
}

/* Statistiques basiques */
void statistiquesEmprunts(BaseEmprunts *be, Bibliotheque *b, BaseUtilisateurs *bu) {
    if (!b || !be || !bu) { printf("Données manquantes pour statistiques.\n"); return; }

    int topN = 5;
    int n = (int)b->nb;
    if (n == 0) { printf("Aucun livre.\n"); return; }

    /* copie des livres */
    Livre *copy = malloc(sizeof(Livre) * n);
    if (!copy) { printf("Mémoire insuffisante.\n"); return; }
    for (int i = 0; i < n; ++i) copy[i] = b->livres[i];

    /* tri décroissant par nb_Emprunts */
    for (int i = 0; i < n-1; ++i)
        for (int j = i+1; j < n; ++j)
            if (copy[j].nb_Emprunts > copy[i].nb_Emprunts) {
                Livre tmp = copy[i]; copy[i] = copy[j]; copy[j] = tmp;
            }

    printf("\nTop %d livres les plus empruntés:\n", topN);
    for (int i = 0; i < n && i < topN; ++i)
        printf("%d) %s (%s) - %d emprunts\n", i+1, copy[i].titre, copy[i].isbn, copy[i].nb_Emprunts);

    free(copy);

    /* Utilisateur le plus actif */
    if (bu->nbUtilisateurs == 0) { printf("Aucun utilisateur.\n"); return; }
    int mu = bu->nbUtilisateurs;
    int *counts = calloc(mu, sizeof(int));
    if (!counts) { printf("Mémoire insuffisante.\n"); return; }

    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        /* trouver index utilisateur dans bu */
        for (int j = 0; j < bu->nbUtilisateurs; ++j) {
            if (bu->utilisateurs[j].id == e->idUtilisateur) { counts[j]++; break; }
        }
    }
    int idxMax = 0;
    for (int j = 1; j < mu; ++j) if (counts[j] > counts[idxMax]) idxMax = j;

    printf("Utilisateur le plus actif : %s %s (ID:%d) - %d emprunts\n",
           bu->utilisateurs[idxMax].prenom,
           bu->utilisateurs[idxMax].nom,
           bu->utilisateurs[idxMax].id,
           counts[idxMax]);

    free(counts);
}

/* ---------- sauvegarde / rechargement (CSV simple) ---------- */

StatutEmprunt sauvegarderEmprunts(BaseEmprunts *be, const char *filename) {
    if (!be || !filename) return EMPRUNT_INEXISTANT;
    FILE *f = fopen(filename, "w");
    if (!f) return EMPRUNT_INEXISTANT;

    /* format : idEmprunt,idUtilisateur,isbn,dateEmprunt_j-m-a,dateRetour_j-m-a,actif,amende\n */
    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        fprintf(f, "%d,%d,%s,%02d-%02d-%04d,%02d-%02d-%04d,%d,%.2f\n",
                e->idEmprunt,
                e->idUtilisateur,
                e->isbn,
                e->dateEmprunt.j, e->dateEmprunt.m, e->dateEmprunt.a,
                e->dateRetour.j, e->dateRetour.m, e->dateRetour.a,
                e->actif,
                e->amende);
    }
    fclose(f);
    return EMPRUNT_OK;
}

static void parseDateToken(const char *tok, Date *d) {
    if (!tok || !d) { d->j = d->m = d->a = 0; return; }
    int j=0,m=0,a=0;
    if (sscanf(tok, "%d-%d-%d", &j, &m, &a) == 3) { d->j=j; d->m=m; d->a=a; }
    else { d->j = d->m = d->a = 0; }
}

StatutEmprunt rechargerEmprunts(BaseEmprunts *be, const char *filename) {
    if (!be || !filename) return EMPRUNT_INEXISTANT;
    FILE *f = fopen(filename, "r");
    if (!f) return EMPRUNT_INEXISTANT;

    char line[512];
    be->nbEmprunts = 0;
    be->nextId = 1;

    while (fgets(line, sizeof(line), f)) {
        if (be->nbEmprunts >= MAX_EMPRUNTS) break;
        Emprunt e = {0};
        char dateE[32] = {0}, dateR[32] = {0};
        int idEmpr=0, idUser=0, actif=0;
        double am = 0.0;
        if (sscanf(line, "%d,%d,%17[^,],%31[^,],%31[^,],%d,%lf",
                   &idEmpr, &idUser, e.isbn, dateE, dateR, &actif, &am) >= 6) {
            e.idEmprunt = idEmpr;
            e.idUtilisateur = idUser;
            parseDateToken(dateE, &e.dateEmprunt);
            parseDateToken(dateR, &e.dateRetour);
            e.actif = actif;
            e.amende = am;
            be->tab[be->nbEmprunts++] = e;
            if (e.idEmprunt >= be->nextId) be->nextId = e.idEmprunt + 1;
        }
    }
    fclose(f);
    return EMPRUNT_OK;
}

/* ---------- menu console pour emprunts (utilise fonctions ci-dessus) ---------- */

void afficherMenuEmprunt(Bibliotheque *b, BaseUtilisateurs *bu, BaseEmprunts *be) {
    if (!b || !bu || !be) {
        printf("Données manquantes (bibliotheque/utilisateurs/emprunts).\n");
        return;
    }

    int choix = -1;
    do {
        printf("\n=== MENU EMPRUNTS ===\n");
        printf("1. Emprunter un livre\n");
        printf("2. Retourner un livre (par ID emprunt)\n");
        printf("3. Lister emprunts actifs\n");
        printf("4. Detecter retards\n");
        printf("5. Statistiques emprunts\n");
        printf("6. Sauvegarder emprunts\n");
        printf("7. Recharger emprunts\n");
        printf("0. Retour\n");
        printf("Choix : ");
        if (scanf("%d", &choix) != 1) { while (getchar() != '\n');
         printf("Entrée invalide.\n"); continue; }
        getchar();

        if (choix == 1) {
            char isbn[ISBN_LEN];
            int uid;
            printf("ISBN : "); fgets(isbn, sizeof(isbn), stdin); isbn[strcspn(isbn, "\n")] = '\0';
            printf("ID utilisateur (entier) : "); scanf("%d", &uid); getchar();
            StatutEmprunt st = emprunterLivre(be, b, bu, isbn, uid);
            if (st == EMPRUNT_OK) printf("Emprunt enregistré.\n");
            else if (st == EMPRUNT_PLUS_DISPONIBLE) printf("Aucun exemplaire disponible.\n");
            else if (st == EMPRUNT_UTILISATEUR_INEXISTANT) printf("Utilisateur introuvable.\n");
            else printf("Erreur emprunt (code %d).\n", st);
        }
        else if (choix == 2) {
            int id;
            printf("ID emprunt à retourner : "); scanf("%d", &id); getchar();
            StatutEmprunt st = retournerLivre(be, b, id);
            if (st == EMPRUNT_OK) printf("Retour enregistré.\n");
            else printf("Erreur lors du retour (code %d).\n", st);
        }
        else if (choix == 3) {
            listerEmpruntsActifs(be);
        }
        else if (choix == 4) {
            detecterRetards(be);
        }
        else if (choix == 5) {
            statistiquesEmprunts(be, b, bu);
        }
        else if (choix == 6) {
            StatutEmprunt s = sauvegarderEmprunts(be, "emprunts.csv");
            if (s == EMPRUNT_OK) printf("Emprunts sauvegardés.\n");
            else printf("Erreur sauvegarde emprunts (code %d).\n", s);
        }
        else if (choix == 7) {
            StatutEmprunt s = rechargerEmprunts(be, "emprunts.csv");
            if (s == EMPRUNT_OK) printf("Emprunts rechargés.\n");
            else printf("Erreur rechargement emprunts (code %d).\n", s);
        }

    } while (choix != 0);
}
#include "emprunt.h"
#include "bibliotheque.h"
#include "utilisateur.h"

// Sauvegarde globale : livres + utilisateurs + emprunts
void sauvegarderToutesDonnees(Bibliotheque *b, BaseUtilisateurs *bu, BaseEmprunts *be)
{
    if (b) {
        StatutLivre s = sauvegarderBibliotheque(b, "livres.txt");
        if (s != STATUT_OK)
            printf("❌ Erreur sauvegarde livres (code %d)\n", s);
    }

    if (bu) {
        StatutUtilisateur s = sauvegarderUtilisateurs(bu, "utilisateurs.csv");
        if (s != OK)
            printf("❌ Erreur sauvegarde utilisateurs (code %d)\n", s);
    }

    if (be) {
        StatutEmprunt s = sauvegarderEmprunts(be, "emprunts.csv");
        if (s != EMPRUNT_OK)
            printf("❌ Erreur sauvegarde emprunts (code %d)\n", s);
    }

    printf("💾 Sauvegarde globale terminée.\n");
}


// Chargement global
void chargerToutesDonnees(Bibliotheque *b, BaseUtilisateurs *bu, BaseEmprunts *be)
{
    if (b) {
        StatutLivre s = chargerBibliotheque(b, "livres.txt");
        if (s != STATUT_OK)
            printf("❌ Erreur chargement livres (code %d)\n", s);
    }

    if (bu) {
        StatutUtilisateur s = chargerUtilisateurs(bu, "utilisateurs.csv");
        if (s != OK)
            printf("❌ Erreur chargement utilisateurs (code %d)\n", s);
    }

    if (be) {
        StatutEmprunt s = rechargerEmprunts(be, "emprunts.csv");
        if (s != EMPRUNT_OK)
            printf("❌ Erreur chargement emprunts (code %d)\n", s);
    }

    printf("📂 Chargement global terminé.\n");
}
