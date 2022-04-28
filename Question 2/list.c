#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// Boolean : 0 = true ; 1 = false

List *createList(int size)
{
    List *list = (List *)malloc(sizeof(List));
    list->size = size;
    list->head = NULL;
    return list;
}

int listIsEmpty(List *list)
{
    if (list->head == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void addFirst(List *list, int value, char* pseudo)
{
    if (list->size == 0){
        printf("There is no more space in the server !\n");
    }
    else {
        Link *link = (Link *)malloc(sizeof(Link));
        link->value = value;
        link->pseudo = pseudo;
        if (list->head == NULL)
        {
            list->head = link;
            link->next = NULL;
        }
        else
        {
            link->next = list->head;
            list->head = link;
        }
        list->size = list->size - 1;
    }
}

Link *next(Link *link)
{
    if (link->next == NULL)
    {
        return NULL;
    }
    else
    {
        return link->next;
    }
}

void delFirst(List *list)
{
    if (list->head != NULL)
    {
        Link *deleted = list->head;
        printf("id supp : %d\n", deleted->value);
        list->head = list->head->next;
        free(deleted);
    }
}

void delVal(List *list, int val)
{
    if (listIsEmpty(list) == 1)
    {
        if (list->head->value == val)
        {
            delFirst(list);
        }
        else
        {
            delValAux(list->head, val);
        }
        list->size = list->size + 1;
    }
    else
    {
        printf("list of sockets is empty !");
    }
}

void delValAux(Link *link, int val)
{
    if (link->next != NULL)
    {
        if (link->next->value == val)
        {
            printf("id supprimer : %d\n", val);
            Link *deleted = link->next;
            link->next = link->next->next;
            free(deleted);
        }
        else
        {
            delValAux(link->next, val);
        }
    }
    else
    {
        printf("Value not found !");
    }
}

int pseudoInList(List* list, char* pseudo){
    if(listIsEmpty(list) == 1){
        Link* current = list->head;
        printf("pseudo current bfr: %s\n", current->pseudo);
        printf("pseudo : %s\n", pseudo);
        while (current != NULL && (strcmp(current->pseudo,pseudo) != 0)){
            printf("pseudo current : %s\n", current->pseudo);
            current = current->next;
        }
        if (current != NULL && (strcmp(current->pseudo,pseudo) == 0)){
            return 0;
        }
        else {
            return 1;
        }
    } else {
        return 1;
    }
}

void displayList(List* list){
    if (listIsEmpty(list) == 1){
        Link* current = list->head;
        while (current != NULL){
            printf("value : %d\n", current->value);
            current = current->next;
        }
    }
    else {
        printf("The list is empty\n");
    }
}