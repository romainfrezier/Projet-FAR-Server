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

void addFirst(List *list, int value, char *pseudo)
{
    if (list->size == 0)
    {
        printf("There is no more space in the server !\n");
    }
    else
    {
        Link *link = (Link *)malloc(sizeof(Link));
        link->value = value;
        link->pseudo = pseudo;
        link->admin = -1;
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

int pseudoInList(List *list, char *pseudo)
{
    if (listIsEmpty(list) == 1)
    {
        Link *current = list->head;
        while (current != NULL && (strcmp(current->pseudo, pseudo) != 0))
        {
            current = current->next;
        }
        if (current != NULL && (strcmp(current->pseudo, pseudo) == 0))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}

int getIdByPseudo(List *list, char *pseudo)
{
    if (listIsEmpty(list) == 1)
    {
        Link *current = list->head;
        while (current != NULL && (strcmp(current->pseudo, pseudo) != 0))
        {
            current = current->next;
        }
        if (current != NULL && (strcmp(current->pseudo, pseudo) == 0))
        {
            return current->value;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

char *getPseudoById(List *list, int id)
{
    if (listIsEmpty(list) == 1)
    {
        Link *current = list->head;
        while (current != NULL && id != current->value)
        {
            current = current->next;
        }
        if (current != NULL && current->value == id)
        {
            return current->pseudo;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

void displayList(List *list)
{
    if (listIsEmpty(list) == 1)
    {
        Link *current = list->head;
        while (current != NULL)
        {    
            printf("Pseudo : %s (id : %d)\n", current->pseudo, current->value);
            current = current->next;
        }
    }
    else
    {
        printf("The list is empty\n");
    }
}

void setUserAdmin(List* list, int idUser){
    Link* current = list->head;
    while (current != NULL && current->value != idUser){
        current = current->next;
    }
    if (current->value == idUser){
        current->admin = 1;
    }
}

int isClientAdmin(List* list, int idUser){
    Link* current = list->head;
    while (current != NULL && current->value != idUser){
        current = current->next;
    }
    if (current->value == idUser){
        return current->admin;
    }
    else {
        return -1;
    }
}

char* getAllUsers(List* list){
    Link* current = list->head;
    char* start = "Users connected : \n";
    char* users = (char*)malloc(strlen(start));
    strcpy(users, start);
    while(current != NULL){
        char *stringUser = (char *)malloc(strlen(current->pseudo) + strlen("(admin) "));
        int taille = 0;
        if (current->admin == 1){
            strcat(stringUser, "(admin) ");
            taille += 8;
        }
        strcat(stringUser, current->pseudo);
        taille += strlen(current->pseudo);
        if (current->next != NULL){
            strcat(stringUser, ", ");
            taille += 2;
        }
        users = realloc(users, strlen(users) + taille);
        strcat(users, stringUser);
        current = current->next;
    }
    return users;
}