#include <stdio.h>
#include <stdlib.h>

typedef struct Maillon Maillon;



struct Maillon {
    int valeur;
    Maillon* suite;
};

typedef struct Liste Liste;

struct Liste{
    Maillon* head;
};

Liste* creerListe(){
    Liste* liste = (Liste*)malloc(sizeof(Liste));
    liste->head = NULL;
    return liste;
}

int listeEstVide(Liste* liste){
    if (liste->head == NULL){
        return 0;
    }
    else {
        return 1;
    }
}

void ajouterDebut(Liste* liste, int entier){
    Maillon* maillon = (Maillon*)malloc(sizeof(Maillon));
    maillon->valeur = entier;
    if (liste->head == NULL){
        liste->head = maillon;
        maillon->suite = NULL;
    }
    else {
        maillon->suite = liste->head;
        liste->head = maillon;
    }
}


Maillon* suivant(Maillon* maillon){
    if (maillon->suite == NULL){
        return NULL;

    }
    else{
        return maillon->suite;
    }
}

void supprimerDebut(Liste* liste){
    if (liste->head != NULL){
        Maillon* supp = liste->head;
        liste->head = liste->head->suite;
        free(supp);
    }
}