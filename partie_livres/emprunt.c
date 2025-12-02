#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "emprunt.h"
#include "utilisateur.h"

/* ===================================================== */
/*                    OUTILS SUR LES DATES               */
/* ===================================================== */

void initDateAujourdHui(Date *d) {
    if (!d) return;
    time_t t = time(NULL);
    struct tm *tmv = localtime(&t);
    if (!tmv) { d->j = d->m = d->a = 0; return; }
    d->j = tmv->tm_mday;
    d->m = tmv->tm_mon + 1;
    d->a = tmv->tm_year + 1900;
}

/* conversion Date -> nombre de jours (algorithme simple) */
static long dateToDays(Date d) {
    int y = d.a;
    int m = d.m;
    int day = d.j;

    if (m < 3) {
        y--;
        m += 12;
    }
    long k = 365L * y + y / 4 - y / 100 + y / 400;
    long mois = (153L * (m - 3) + 2) / 5;
    return k + mois + day;
}

/* nombre de jours entre debut et fin = fin - debut (peut être négatif) */
int differenceJours(Date debut, Date fin) {
    long a = dateToDays(debut);
    long b = dateToDays(fin);
    return (int)(b - a);
}

/* demande une date à l'utilisateur */
Date lireDateDepuisClavier(const char *message) {
    Date d = {0,0,0};
    int ok = 0;
    char buffer[64];

    do {
        printf("%s", message ? message: "Date (jj/mm/aaaa): ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            d.j = d.m = d.a = 0;
            return d;
        }
        if (sscanf(buffer, "%d/%d/%d", &d.j, &d.m, &d.a) == 3) {
            ok = 1;
        } else {
            printf("Format invalide. Exemple : 05/12/2025\n");
        }
    } while (!ok);

    return d;
}

/* ===================================================== */
/*                 HELPERS INTERNES                      */
/* ===================================================== */

/* Cherche un livre par ISBN -> index ou -1 */
static int indexLivreParISBN(const Bibliotheque *b, const char *isbn) {
    if (!b || !isbn) return -1;
    for (size_t i = 0; i < b->nb; ++i) {
        if (strcmp(b->livres[i].isbn, isbn) == 0)
            return (int)i;
    }
    return -1;
}

/* vérifie l'existence d'un utilisateur */
static int utilisateurExiste(BaseUtilisateurs *bu, int userId) {
    if (!bu) return 0;
    for (int i = 0; i < bu->nbUtilisateurs; ++i)
        if (bu->utilisateurs[i].id == userId)
            return 1;
    return 0;
}

/* ===================================================== */
/*             COMPTEURS EMPRUNTS / PENALITES            */
/* ===================================================== */

int compterEmpruntsActifs(BaseEmprunts *be, int userId) {
    if (!be) return 0;
    int c = 0;
    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        if (e->actif && e->idUtilisateur == userId)
            c++;
    }
    return c;
}

/* une pénalité = un emprunt rendu en retard (amende > 0) */
int compterPenalites(BaseEmprunts *be, int userId) {
    if (!be) return 0;
    int c = 0;
    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        if (!e->actif &&
            e->idUtilisateur == userId &&
            e->amende > 0.0)
        {
            c++;
        }
    }
    return c;
}

/* ===================================================== */
/*               LOGIQUE EMPRUNT / RETOUR                */
/* ===================================================== */

/* Emprunter : vérifie livre, utilisateur, disponibilité,
 * quota dynamique (5 - pénalités) et crée l'emprunt      */
StatutEmprunt emprunterLivre(BaseEmprunts *be,
                             Bibliotheque *b,
                             BaseUtilisateurs *bu,
                             const char *isbn,
                             int userId)
{
    if (!be || !b || !bu || !isbn) return EMPRUNT_INEXISTANT;

    int idxL = indexLivreParISBN(b, isbn);
    if (idxL < 0) return EMPRUNT_INEXISTANT;

    if (!utilisateurExiste(bu, userId))
        return EMPRUNT_UTILISATEUR_INEXISTANT;

    Livre *L = &b->livres[idxL];

    if (L->nb_exemplaires_disponibles <= 0)
        return EMPRUNT_PLUS_DISPONIBLE;

    /* calcul du quota dynamique : */
    int nbPenalites = compterPenalites(be, userId);
    int quota = MAX_PRETS_PAR_USER - nbPenalites;
    if (quota <= 0) {
        return EMPRUNT_QUOTA_ATTEINT;
    }

    int actifs = compterEmpruntsActifs(be, userId);
    if (actifs >= quota)
        return EMPRUNT_QUOTA_ATTEINT;

    if (be->nbEmprunts >= MAX_EMPRUNTS)
        return BASE_EMPRUNT_PLEINE;

    /* création de l'emprunt */
    Emprunt e;
    e.idEmprunt = be->nextId++;
    e.idUtilisateur = userId;
    strncpy(e.isbn, isbn, ISBN_LEN - 1);
    e.isbn[ISBN_LEN - 1] = '\0';

    /* 🔹 l'utilisateur choisit la date d'emprunt */
    e.dateEmprunt = lireDateDepuisClavier("Date d'emprunt (jj/mm/aaaa) : ");

    e.dateRetour.j = e.dateRetour.m = e.dateRetour.a = 0;
    e.actif = 1;
    e.amende = 0.0;
    e.numExemplaire = 0;

    be->tab[be->nbEmprunts++] = e;

    /* maj du livre */
    L->nb_exemplaires_disponibles--;
    if (L->nb_exemplaires_disponibles == 0)
        L->dispo = LIVRE_EMPRUNTE;
    L->nb_Emprunts++;

    return EMPRUNT_OK;
}

/* Retour d'un livre : calcul de la pénalité éventuelle,
 * incrément de l'exemplaire disponible et historisation  */
StatutEmprunt retournerLivre(BaseEmprunts *be,
                             Bibliotheque *b,
                             int empruntId)
{
    if (!be || !b) return EMPRUNT_INEXISTANT;

    int pos = -1;
    for (int i = 0; i < be->nbEmprunts; ++i) {
        if (be->tab[i].idEmprunt == empruntId && be->tab[i].actif) {
            pos = i;
            break;
        }
    }
    if (pos == -1) return EMPRUNT_INEXISTANT;

    Emprunt *e = &be->tab[pos];

    printf("Retour du livre ISBN %s (emprunté le %02d/%02d/%04d)\n",
           e->isbn, e->dateEmprunt.j, e->dateEmprunt.m, e->dateEmprunt.a);

    /* on demande la date de retour à l'utilisateur */
    Date dr = lireDateDepuisClavier("Date de retour (jj/mm/aaaa) : ");
    e->dateRetour = dr;
    e->actif = 0;

    /* mise à jour du livre */
    int idxL = indexLivreParISBN(b, e->isbn);
    if (idxL >= 0) {
        Livre *L = &b->livres[idxL];
        if (L->nb_exemplaires_disponibles < L->nb_exemplaires_total)
            L->nb_exemplaires_disponibles++;
        if (L->nb_exemplaires_disponibles > 0)
            L->dispo = LIVRE_DISPONIBLE;
    }

    /* calcul retard / amende */
    int duree = differenceJours(e->dateEmprunt, dr);   // nb de jours entre emprunt et retour
    if (duree > DELAI_RETARD_JOURS) {
        int joursRetard = duree - DELAI_RETARD_JOURS;
        e->amende = (double)joursRetard * AMENDE_PAR_JOUR;
        printf("⏰ Retour en retard de %d jours -> amende = %.2f €\n",
               joursRetard, e->amende);
        printf("Cette pénalité réduira votre quota d'emprunts futurs.\n");
    } else {
        e->amende = 0.0;
        printf("✅ Retour dans les délais, aucune amende.\n");
    }

    return EMPRUNT_OK;
}

/* ===================================================== */
/*               AFFICHAGE / RETARDS / STATS             */
/* ===================================================== */

void listerEmpruntsActifs(BaseEmprunts *be) {
    if (!be) { printf("Aucune base d'emprunts.\n"); return; }

    printf("\n--- Emprunts actifs ---\n");
    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        if (e->actif) {
            printf("ID:%d | ISBN:%s | UserID:%d | Date emprunt:%02d/%02d/%04d\n",
                   e->idEmprunt, e->isbn, e->idUtilisateur,
                   e->dateEmprunt.j, e->dateEmprunt.m, e->dateEmprunt.a);
        }
    }
}

/* Case 4 du menu : afficher les livres en retard de retour */
void detecterRetards(BaseEmprunts *be) {
    if (!be) { printf("Aucune base d'emprunts.\n"); return; }

    Date aujourd;
    initDateAujourdHui(&aujourd);

    printf("\n--- Emprunts en retard (> %d jours) ---\n", DELAI_RETARD_JOURS);
    int nb = 0;

    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        if (!e->actif) continue;  // on ne regarde que les livres non rendus

        int duree = differenceJours(e->dateEmprunt, aujourd); // jours depuis l'emprunt
        if (duree > DELAI_RETARD_JOURS) {
            int joursRetard = duree - DELAI_RETARD_JOURS;
            printf("ID Emprunt:%d | ISBN:%s | UserID:%d | Emprunté le %02d/%02d/%04d | Retard: %d jours\n",
                   e->idEmprunt,
                   e->isbn,
                   e->idUtilisateur,
                   e->dateEmprunt.j, e->dateEmprunt.m, e->dateEmprunt.a,
                   joursRetard);
            nb++;
        }
    }

    if (nb == 0) {
        printf("Aucun emprunt en retard.\n");
    }
}

/* statistiques (top livres + utilisateur le plus actif) */
void statistiquesEmprunts(BaseEmprunts *be,
                          Bibliotheque *b,
                          BaseUtilisateurs *bu)
{
    if (!b || !be || !bu) {
        printf("Données manquantes pour statistiques.\n");
        return;
    }

    int n = (int)b->nb;
    if (n == 0) { printf("Aucun livre.\n"); return; }

    Livre *copy = malloc(sizeof(Livre) * n);
    if (!copy) { printf("Mémoire insuffisante.\n"); return; }
    for (int i = 0; i < n; ++i) copy[i] = b->livres[i];

    /* tri décroissant sur nb_Emprunts */
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (copy[j].nb_Emprunts > copy[i].nb_Emprunts) {
                Livre tmp = copy[i];
                copy[i] = copy[j];
                copy[j] = tmp;
            }
        }
    }

    int topN = (n < 5) ? n : 5;
    printf("\nTop %d livres les plus empruntés :\n", topN);
    for (int i = 0; i < topN; ++i) {
        printf("%d) %s (%s) - %d emprunts\n",
               i + 1,
               copy[i].titre,
               copy[i].isbn,
               copy[i].nb_Emprunts);
    }
    free(copy);

    if (bu->nbUtilisateurs == 0) { printf("Aucun utilisateur.\n"); return; }

    int mu = bu->nbUtilisateurs;
    int *counts = calloc(mu, sizeof(int));
    if (!counts) { printf("Mémoire insuffisante.\n"); return; }

    for (int i = 0; i < be->nbEmprunts; ++i) {
        Emprunt *e = &be->tab[i];
        for (int j = 0; j < mu; ++j) {
            if (bu->utilisateurs[j].id == e->idUtilisateur) {
                counts[j]++;
                break;
            }
        }
    }

    int idxMax = 0;
    for (int j = 1; j < mu; ++j)
        if (counts[j] > counts[idxMax]) idxMax = j;

    printf("Utilisateur le plus actif : %s %s (ID:%d) - %d emprunts\n",
           bu->utilisateurs[idxMax].prenom,
           bu->utilisateurs[idxMax].nom,
           bu->utilisateurs[idxMax].id,
           counts[idxMax]);

    free(counts);
}

/* ===================================================== */
/*         SAUVEGARDE / RECHARGEMENT DES EMPRUNTS        */
/* ===================================================== */

StatutEmprunt sauvegarderEmprunts(BaseEmprunts *be, const char *filename) {
    if (!be || !filename) return EMPRUNT_INEXISTANT;
    FILE *f = fopen(filename, "w");
    if (!f) return EMPRUNT_INEXISTANT;

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
    int j = 0, m = 0, a = 0;
    if (sscanf(tok, "%d-%d-%d", &j, &m, &a) == 3) {
        d->j = j; d->m = m; d->a = a;
    } else {
        d->j = d->m = d->a = 0;
    }
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
        int idEmpr = 0, idUser = 0, actif = 0;
        double am = 0.0;

        if (sscanf(line, "%d,%d,%17[^,],%31[^,],%31[^,],%d,%lf",
                   &idEmpr, &idUser, e.isbn, dateE, dateR, &actif, &am) >= 6)
        {
            e.idEmprunt = idEmpr;
            e.idUtilisateur = idUser;
            parseDateToken(dateE, &e.dateEmprunt);
            parseDateToken(dateR, &e.dateRetour);
            e.actif = actif;
            e.amende = am;

            be->tab[be->nbEmprunts++] = e;
            if (e.idEmprunt >= be->nextId)
                be->nextId = e.idEmprunt + 1;
        }
    }
    fclose(f);
    return EMPRUNT_OK;
}

/* ===================================================== */
/*                 MENU CONSOLE EMPRUNTS                 */
/* ===================================================== */

void afficherMenuEmprunt(Bibliotheque *b, BaseUtilisateurs *bu, BaseEmprunts *be, int role) {
    if (!b || !bu || !be) return;

    int choix = -1;
    do {
        printf("\n=== MENU EMPRUNTS ===\n");
        printf("1. Emprunter un livre\n");
        printf("2. Retourner un livre (par ID emprunt)\n");
        printf("3. Lister emprunts actifs\n");
        printf("4. Détecter retards\n");
        printf("5. Statistiques emprunts\n");
        printf("6. Sauvegarder emprunts\n");
        printf("7. Recharger emprunts\n");
        printf("0. Retour\n");
        printf("Choix : ");

        if(scanf("%d",&choix)!=1){while(getchar()!='\n'); printf("Entrée invalide.\n"); continue;}
        getchar();

        int id, uid; char isbn[ISBN_LEN]; StatutEmprunt st;
        switch(choix) {
            case 1: // Emprunt
                printf("ISBN : "); fgets(isbn,sizeof(isbn),stdin); isbn[strcspn(isbn,"\n")]=0;
                printf("ID utilisateur : "); if(scanf("%d",&uid)!=1){while(getchar()!='\n'); printf("Entrée invalide.\n"); break;} getchar();
                st = emprunterLivre(be,b,bu,isbn,uid);
                printf(st==EMPRUNT_OK?"Emprunt enregistré.\n":"Erreur emprunt (code %d)\n",st);
                break;

            case 2: // Retour
                printf("ID emprunt à retourner : "); if(scanf("%d",&id)!=1){while(getchar()!='\n'); printf("Entrée invalide.\n"); break;} getchar();
                st = retournerLivre(be,b,id);
                printf(st==EMPRUNT_OK?"Retour enregistré.\n":"Erreur retour (code %d)\n",st);
                break;

            case 3: // Lister emprunts actifs (protégé)
                if(role != ROLE_ADMIN){ printf("⚠️ Accès réservé administrateur.\n"); break; }
                listerEmpruntsActifs(be);
                break;

            case 4: // Détecter retards (protégé)
                if(role != ROLE_ADMIN){ printf("⚠️ Accès réservé administrateur.\n"); break; }
                detecterRetards(be);
                break;

            case 5: // Statistiques (protégé)
                if(role != ROLE_ADMIN){ printf("⚠️ Accès réservé administrateur.\n"); break; }
                statistiquesEmprunts(be,b,bu);
                break;

            case 6: // Sauvegarder
                st = sauvegarderEmprunts(be,"emprunts.csv");
                printf(st==EMPRUNT_OK?"💾 Sauvegarde OK\n":"Erreur sauvegarde (code %d)\n",st);
                break;

            case 7: // Recharger
                st = rechargerEmprunts(be,"emprunts.csv");
                printf(st==EMPRUNT_OK?"📂 Rechargement OK\n":"Erreur rechargement (code %d)\n",st);
                break;
        }
    } while(choix!=0);
}


/* ===================================================== */
/*           SAUVEGARDE / CHARGEMENT GLOBAL              */
/* ===================================================== */

void sauvegarderToutesDonnees(Bibliotheque *b,
                              BaseUtilisateurs *bu,
                              BaseEmprunts *be)
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

void chargerToutesDonnees(Bibliotheque *b,
                          BaseUtilisateurs *bu,
                          BaseEmprunts *be)
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