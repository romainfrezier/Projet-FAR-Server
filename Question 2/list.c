#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct Link Link;



struct Link {
    int value;
    Link* next;
};

typedef struct List List;

struct List{
    Link* head;
};

List* createList(){
    List* list = (List*)malloc(sizeof(List));
    list->head = NULL;
    return list;
}

int listIsEmpty(List* list){
    if (list->head == NULL){
        return 0;
    }
    else {
        return 1;
    }
}

void addFirst(List* list, int value){
    Link* link = (Link*)malloc(sizeof(Link));
    link->value = value;
    if (list->head == NULL){
        list->head = link;
        link->next = NULL;
    }
    else {
        link->next = list->head;
        list->head = link;
    }
}


Link* next(Link* link){
    if (link->next == NULL){
        return NULL;
    }
    else{
        return link->next;
    }
}

void delFirst(List* list){
    if (list->head != NULL){
        Link* deleted = list->head;
        list->head = list->head->next;
        free(deleted);
    }
}

void delVal(List* list, int val){
    if (listIsEmpty(list) == 0){
        if (list->head->value == val){
            delFirst(list);
        }
        else {
            delValAux(list->head, val);
        }
    }
    else {
        printf("list of sockets is empty !");
    }
}

void delValAux(Link* link, int val){
    if (link->next != NULL){
        if (link->next->value == val){
            Link* deleted = link->next;
            link->next = link->next->next;
            free(deleted);
        }
        else {
            delValAux(link->next, val);
        }
    }
    else {
        printf("Value not found !");
    }
}