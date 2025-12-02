// utilisateur.c
// Created by marie on 27/11/2025.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "emprunt.h"
#include "utilisateur.h"

/* ---------- helpers ---------- */

/* Conversion en minuscules (in-place) */
void toLowerCase(char *str) {
    for (size_t i = 0; str[i]; ++i)
        str[i] = (char)tolower((unsigned char)str[i]);
}

/* trim in-place : supprime espaces en début et fin */
void trim(char *s) {
    if (!s || *s == '\0') return;
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        --len;
    }
}

/* ---------- recherches / affichage ---------- */

Utilisateur* rechercherUtilisateurParID(BaseUtilisateurs *base, int id) {
    if (!base) return NULL;
    for (int i = 0; i < base->nbUtilisateurs; ++i) {
        if (base->utilisateurs[i].id == id) return &base->utilisateurs[i];
    }
    return NULL;
}

Utilisateur* rechercherUtilisateurParEmail(BaseUtilisateurs *base, const char *email) {
    if (!base || !email) return NULL;
    for (int i = 0; i < base->nbUtilisateurs; ++i) {
        if (strcmp(base->utilisateurs[i].email, email) == 0) return &base->utilisateurs[i];
    }
    return NULL;
}

void rechercherUtilisateurParNom(BaseUtilisateurs *base, const char *nom) {
    if (!base || !nom) return;
    char recherche[256];
    strncpy(recherche, nom, sizeof(recherche)-1);
    recherche[sizeof(recherche)-1] = '\0';
    trim(recherche);
    toLowerCase(recherche);

    int trouve = 0;
    for (int i = 0; i < base->nbUtilisateurs; ++i) {
        char tmp[256];
        strncpy(tmp, base->utilisateurs[i].nom, sizeof(tmp)-1);
        tmp[sizeof(tmp)-1] = '\0';
        toLowerCase(tmp);
        if (strstr(tmp, recherche) != NULL) {
            printf("ID: %d | %s %s | %s\n",
                   base->utilisateurs[i].id,
                   base->utilisateurs[i].prenom,
                   base->utilisateurs[i].nom,
                   base->utilisateurs[i].email);
            trouve = 1;
        }
    }
    if (!trouve) printf("Aucun utilisateur trouvé pour le nom : %s\n", nom);
}

void afficherUtilisateurs(BaseUtilisateurs *base) {
    if (!base || base->nbUtilisateurs == 0) {
        printf("Aucun utilisateur enregistré.\n");
        return;
    }
    printf("\n%-5s | %-15s | %-15s | %-25s\n", "ID", "Nom", "Prénom", "E-mail");
    printf("-----------------------------------------------------------------------\n");
    for (int i = 0; i < base->nbUtilisateurs; ++i) {
        printf("%-5d | %-15s | %-15s | %-25s\n",
               base->utilisateurs[i].id,
               base->utilisateurs[i].nom,
               base->utilisateurs[i].prenom,
               base->utilisateurs[i].email);
    }
}

/* ---------- tri helpers ---------- */
static int comparerParNom(const void *a, const void *b) {
    const Utilisateur *u1 = a;
    const Utilisateur *u2 = b;
    int cmp = strcmp(u1->nom, u2->nom);
    if (cmp == 0) cmp = strcmp(u1->prenom, u2->prenom);
    return cmp;
}

static int comparerParID(const void *a, const void *b) {
    const Utilisateur *u1 = a;
    const Utilisateur *u2 = b;
    return (u1->id - u2->id);
}

void afficherUtilisateursTriesParNom(BaseUtilisateurs *base) {
    if (!base || base->nbUtilisateurs == 0) { printf("Aucun utilisateur à trier.\n"); return; }
    qsort(base->utilisateurs, base->nbUtilisateurs, sizeof(Utilisateur), comparerParNom);
    afficherUtilisateurs(base);
}

void afficherUtilisateursTriesParID(BaseUtilisateurs *base) {
    if (!base || base->nbUtilisateurs == 0) { printf("Aucun utilisateur à trier.\n"); return; }
    qsort(base->utilisateurs, base->nbUtilisateurs, sizeof(Utilisateur), comparerParID);
    afficherUtilisateurs(base);
}

/* ---------- opérations CRUD ---------- */
int existeUtilisateur(BaseUtilisateurs *base, int id) {
    return (rechercherUtilisateurParID(base, id) != NULL);
}

StatutUtilisateur ajouterUtilisateur(BaseUtilisateurs *base, Utilisateur u) {
    if (!base) return BASE_PLEINE;
    if (base->nbUtilisateurs >= MAX_UTILISATEURS) return BASE_PLEINE;
    if (existeUtilisateur(base, u.id)) return UTILISATEUR_EXISTE;

    u.nom[sizeof(u.nom)-1] = '\0';
    u.prenom[sizeof(u.prenom)-1] = '\0';
    u.email[sizeof(u.email)-1] = '\0';

    base->utilisateurs[base->nbUtilisateurs++] = u;
    return OK;
}

StatutUtilisateur modifierUtilisateur(BaseUtilisateurs *base, UpdateUtilisateur upd) {
    if (!base) return UTILISATEUR_INEXISTANT;
    for (int i = 0; i < base->nbUtilisateurs; ++i) {
        if (base->utilisateurs[i].id == upd.id) {
            if (strlen(upd.nom) > 0) strncpy(base->utilisateurs[i].nom, upd.nom, sizeof(base->utilisateurs[i].nom)-1);
            if (strlen(upd.prenom) > 0) strncpy(base->utilisateurs[i].prenom, upd.prenom, sizeof(base->utilisateurs[i].prenom)-1);
            if (strlen(upd.email) > 0) strncpy(base->utilisateurs[i].email, upd.email, sizeof(base->utilisateurs[i].email)-1);
            return OK;
        }
    }
    return UTILISATEUR_INEXISTANT;
}

StatutUtilisateur supprimerUtilisateurParID(BaseUtilisateurs *base, int id) {
    if (!base) return UTILISATEUR_INEXISTANT;
    for (int i = 0; i < base->nbUtilisateurs; ++i) {
        if (base->utilisateurs[i].id == id) {
            for (int j = i; j < base->nbUtilisateurs - 1; ++j)
                base->utilisateurs[j] = base->utilisateurs[j + 1];
            base->nbUtilisateurs--;
            return OK;
        }
    }
    return UTILISATEUR_INEXISTANT;
}

/* ---------- sauvegarde / chargement CSV ---------- */
StatutUtilisateur sauvegarderUtilisateurs(BaseUtilisateurs *base, const char *filename) {
    if (!base || !filename) return BASE_PLEINE;
    FILE *f = fopen(filename, "w");
    if (!f) return BASE_PLEINE;

    for (int i = 0; i < base->nbUtilisateurs; ++i) {
        fprintf(f, "%d,%s,%s,%s\n",
                base->utilisateurs[i].id,
                base->utilisateurs[i].nom,
                base->utilisateurs[i].prenom,
                base->utilisateurs[i].email);
    }
    fclose(f);
    return OK;
}

StatutUtilisateur chargerUtilisateurs(BaseUtilisateurs *base, const char *filename) {
    if (!base || !filename) return BASE_PLEINE;
    FILE *f = fopen(filename, "r");
    if (!f) return BASE_PLEINE;

    char line[512];
    base->nbUtilisateurs = 0;

    while (fgets(line, sizeof(line), f)) {
        if (base->nbUtilisateurs >= MAX_UTILISATEURS) break;

        Utilisateur u = {0};
        char *tok = strtok(line, ",");
        if (!tok) continue;
        u.id = atoi(tok);

        tok = strtok(NULL, ",");
        if (!tok) continue;
        strncpy(u.nom, tok, sizeof(u.nom)-1); u.nom[sizeof(u.nom)-1] = '\0'; u.nom[strcspn(u.nom, "\n")] = '\0';

        tok = strtok(NULL, ",");
        if (!tok) continue;
        strncpy(u.prenom, tok, sizeof(u.prenom)-1); u.prenom[sizeof(u.prenom)-1] = '\0'; u.prenom[strcspn(u.prenom, "\n")] = '\0';

        tok = strtok(NULL, ",");
        if (!tok) continue;
        strncpy(u.email, tok, sizeof(u.email)-1); u.email[sizeof(u.email)-1] = '\0'; u.email[strcspn(u.email, "\n")] = '\0';

        base->utilisateurs[base->nbUtilisateurs++] = u;
    }

    fclose(f);
    return OK;
}

/* ---------- authentification admin simple ---------- */
int authentifierAdmin(void) {
    char pass[128];
    printf("Mot de passe administrateur : ");
    if (scanf("%127s", pass) != 1) return 0;
    int c; while ((c = getchar()) != '\n' && c != EOF) {}
#ifdef ADMIN_PASSWORD
    if (strcmp(pass, ADMIN_PASSWORD) == 0) { printf("Authentification OK.\n"); return 1; }
#endif
    printf("Mot de passe incorrect (ou ADMIN_PASSWORD non défini).\n");
    return 0;
}

/* ---------- menu utilisateur sécurisé ---------- */
void menu_recherche(BaseUtilisateurs *base, int role) {
    if (!base) return;
    int choix;
    do {
        printf("\n===== MENU GESTION UTILISATEURS =====\n");
        printf("1. Ajouter un utilisateur\n");
        printf("2. Modifier un utilisateur\n");
        printf("3. Supprimer un utilisateur\n");
        printf("4. Rechercher un utilisateur par nom\n");
        printf("5. Lister tous les utilisateurs\n");
        printf("6. Lister triés par nom\n");
        printf("7. Lister triés par ID\n");
        printf("8. Sauvegarder (CSV)\n");
        printf("9. Charger (CSV)\n");
        printf("0. Retour\n");
        printf("Votre choix : ");

        if (scanf("%d", &choix) != 1) {
            int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Entrée invalide.\n");
            continue;
        }
        getchar(); // consommer '\n'

        UpdateUtilisateur upd;
        Utilisateur u;
        StatutUtilisateur s;
        int id;

        switch (choix) {
            case 1: // Ajouter

                memset(&u, 0, sizeof(u));
                printf("ID : "); scanf("%d", &u.id); getchar();
                printf("Nom : "); fgets(u.nom, sizeof(u.nom), stdin); trim(u.nom);
                printf("Prénom : "); fgets(u.prenom, sizeof(u.prenom), stdin); trim(u.prenom);
                printf("Email : "); fgets(u.email, sizeof(u.email), stdin); trim(u.email);
                s = ajouterUtilisateur(base, u);
                printf(s == OK ? "✅ Utilisateur ajouté.\n" : "❌ Erreur (code %d).\n", s);
                break;

            case 2: // Modifier
                printf("ID à modifier : "); scanf("%d", &id); getchar();
                Utilisateur *uMod = rechercherUtilisateurParID(base, id);
                if (!uMod) { printf("Utilisateur introuvable.\n"); break; }
                memset(&upd, 0, sizeof(upd));
                upd.id = id;
                printf("Nouveau nom (laisser vide pour garder '%s') : ", uMod->nom);
                fgets(upd.nom, sizeof(upd.nom), stdin); trim(upd.nom);
                printf("Nouveau prénom (laisser vide pour garder '%s') : ", uMod->prenom);
                fgets(upd.prenom, sizeof(upd.prenom), stdin); trim(upd.prenom);
                printf("Nouvel email (laisser vide pour garder '%s') : ", uMod->email);
                fgets(upd.email, sizeof(upd.email), stdin); trim(upd.email);
                s = modifierUtilisateur(base, upd);
                printf(s == OK ? "✅ Modifié avec succès.\n" : "❌ Erreur (code %d).\n", s);
                break;

            case 3: // Supprimer
                if (role != ROLE_ADMIN) { printf("⚠️ Accès refusé.\n"); break; }
                printf("ID à supprimer : "); scanf("%d", &id); getchar();
                s = supprimerUtilisateurParID(base, id);
                printf(s == OK ? "✅ Supprimé.\n" : "❌ Erreur (code %d).\n", s);
                break;

            case 4: // Rechercher par nom
                { if (role != ROLE_ADMIN) { printf("⚠️ Accès refusé.\n"); break; }
                    char nom[128];
                    printf("Nom à rechercher : ");
                    fgets(nom, sizeof(nom), stdin); trim(nom);
                    rechercherUtilisateurParNom(base, nom);
                }
                break;

            case 5:  if (role != ROLE_ADMIN) { printf("⚠️ Accès refusé.\n"); break; }
                afficherUtilisateurs(base); break;
            case 6: if (role != ROLE_ADMIN) { printf("⚠️ Accès refusé.\n"); break; }
                afficherUtilisateursTriesParNom(base); break;
            case 7: if (role != ROLE_ADMIN) { printf("⚠️ Accès refusé.\n"); break; }
                afficherUtilisateursTriesParID(base); break;

            case 8: // Sauvegarder

                s = sauvegarderUtilisateurs(base, "utilisateurs.csv");
                printf(s == OK ? "💾 Sauvegarde réussie (utilisateurs.csv)\n" : "❌ Erreur (code %d)\n", s);
                break;

            case 9: // Charger

                s = chargerUtilisateurs(base, "utilisateurs.csv");
                printf(s == OK ? "📂 Chargement réussi (utilisateurs.csv)\n" : "❌ Erreur (code %d)\n", s);
                break;

            case 0: break;
            default: printf("Choix invalide.\n");
        }

    } while (choix != 0);

    printf("👋 Retour menu principal.\n");
}

/* ---------- sélection rôle admin / utilisateur ---------- */
int Admin(Utilisateur *u) {
    int choix;
    printf("\n===== AUTHENTIFICATION =====\n");
    printf("1. Administrateur\n2. Utilisateur simple\nChoix : ");
    if (scanf("%d", &choix) != 1) return 0;
    getchar(); // consommer '\n'

    if (choix == 1) {
        char pass[128];
        printf("Mot de passe administrateur : ");
        scanf("%127s", pass);
        if (strcmp(pass, ADMIN_PASSWORD) == 0) {
            printf("✔️ Administrateur connecté.\n");
            return ROLE_ADMIN;
        } else {
            printf("❌ Mot de passe incorrect.\n");
            return 0;
        }
    } else if (choix == 2) {
        printf("👤 Connexion utilisateur simple.\n");
        return ROLE_UTILISATEUR;
    }
    return 0;
}
