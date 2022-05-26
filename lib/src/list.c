/**
 * @file list.c
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief list of clients functions implementation
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/list.h"

// Boolean : 0 = true ; 1 = false

// create an empty list
List *createList(int size)
{
    List *list = (List *)malloc(sizeof(List));
    list->size = size;
    list->head = NULL;
    return list;
}

// check if the list is empty. Return 0 if the list is empty
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

// add a user to the first position of the list
void addFirst(List *list, int id, char *pseudo)
{
    if (list->size == 0)
    {
        printf("There is no more space in the server !\n");
    }
    else
    {
        Client *link = (Client *)malloc(sizeof(Client));
        link->id = id;
        link->pseudo = pseudo;
        link->admin = -1;
        link->alreadyConnected = 0;
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

void addFirstClient(List *list, Client *client, char *pseudo)
{
    if (list->size == 0)
    {
        printf("There is no more space in the server !\n");
    }
    else
    {
        Client *link = (Client *)malloc(sizeof(Client));
        link->id = client->id;
        link->pseudo = pseudo;
        link->admin = client->admin;
        link->alreadyConnected = client->alreadyConnected;
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

// go to the next Link of the list
Client *next(Client *client)
{
    if (client->next == NULL)
    {
        return NULL;
    }
    else
    {
        return client->next;
    }
}

// delete the first element of the list
void delFirst(List *list)
{
    if (list->head != NULL)
    {
        Client *deleted = list->head;
        list->head = list->head->next;
        free(deleted);
    }
}

// delete an element of the list passed in arguments
void delVal(List *list, int id)
{
    if (listIsEmpty(list) == 1)
    {
        if (list->head->id == id)
        {
            delFirst(list);
        }
        else
        {
            delValAux(list->head, id);
        }
        list->size = list->size + 1;
    }
    else
    {
        printf("list of sockets is empty !");
    }
}

// recursive function to delete an element of the list
void delValAux(Client *client, int id)
{
    if (client->next != NULL)
    {
        if (client->next->id == id)
        {
            Client *deleted = client->next;
            client->next = client->next->next;
            free(deleted);
        }
        else
        {
            delValAux(client->next, id);
        }
    }
    else
    {
        printf("Value not found !");
    }
}

// chek if the pseudo is in the list. Return 0 if the pseudo is in the list
int pseudoInList(List *list, char *pseudo)
{
    if (listIsEmpty(list) == 1)
    {
        Client *current = list->head;
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

// get the id from the pseudo
int getIdByPseudo(List *list, char *pseudo)
{
    if (listIsEmpty(list) == 1)
    {
        Client *current = list->head;
        while (current != NULL && (strcmp(current->pseudo, pseudo) != 0))
        {
            current = current->next;
        }
        if (current != NULL && (strcmp(current->pseudo, pseudo) == 0))
        {
            return current->id;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

// get the pseudo from the id
char *getPseudoById(List *list, int id)
{
    if (listIsEmpty(list) == 1)
    {
        Client *current = list->head;
        while (current != NULL && id != current->id)
        {
            current = current->next;
        }
        if (current != NULL && current->id == id)
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

// display the list
void displayList(List *list)
{
    if (listIsEmpty(list) == 1)
    {
        Client *current = list->head;
        while (current != NULL)
        {    
            printf("Pseudo : %s (id : %d)\n", current->pseudo, current->id);
            current = current->next;
        }
    }
    else
    {
        printf("The list is empty\n");
    }
}

// make a user as an admin
void setUserAdmin(List* list, int idUser){
    Client* current = list->head;
    while (current != NULL && current->id != idUser){
        current = current->next;
    }
    if (current != NULL){
        current->admin = 1;
    }
}

// check if the user is an admin
int isUserAdmin(List* list, int idUser){
    Client* current = list->head;
    while (current != NULL && current->id != idUser){
        current = current->next;
    }
    if (current->id == idUser){
        return current->admin;
    }
    else {
        return -1;
    }
}

// get all the users of the list
char *getAllChannelUsers(List *list, int idClient, char *start)
{
    Client* current = list->head;

    // List of different components of the user list
    char *admin = "(admin)";
    char *me = "(me)";
    char *tab = "\t• ";
    char *space = " ";
    char *retn = "\n";

    char *users = (char *)malloc(strlen(start));
    strcpy(users, start);
    
    // add every client to the final string
    while(current != NULL){
        int taille = 0;
        char *stringUser;
        if (current->admin == 1 && current->id == idClient)
        {
            stringUser = (char *)malloc(strlen(current->pseudo) + strlen(tab) + strlen(admin) + strlen(me) + strlen(space) + strlen(retn));
            strcat(stringUser, tab);
            strcat(stringUser, me);
            strcat(stringUser, admin);
            strcat(stringUser, space);
            taille += strlen(tab) + strlen(admin) + strlen(me) + strlen(space) + strlen(retn);
        }
        else if (current->id == idClient && current->admin != 1)
        {
            stringUser = (char *)malloc(strlen(current->pseudo) + strlen(tab) + strlen(me) + strlen(space) + strlen(retn));
            strcat(stringUser, tab);
            strcat(stringUser, me);
            strcat(stringUser, space);
            taille += strlen(tab) + strlen(me) + strlen(space) + strlen(retn);
        }
        else if (current->id != idClient && current->admin == 1)
        {
            stringUser = (char *)malloc(strlen(current->pseudo) + strlen(tab) + strlen(admin) + strlen(space) + strlen(retn));
            strcat(stringUser, tab);
            strcat(stringUser, admin);
            strcat(stringUser, space);
            taille += strlen(tab) + strlen(admin) + strlen(space) + strlen(retn);
        } 
        else {
            stringUser = (char *)malloc(strlen(current->pseudo) + strlen(tab) + strlen(me) + strlen(space) + strlen(retn));
            strcat(stringUser, tab);
            taille += strlen(tab) + strlen(retn);
        }
        strcat(stringUser, current->pseudo);
        strcat(stringUser, retn);
        taille += strlen(current->pseudo);
        users = realloc(users, strlen(users) + taille);
        strcat(users, stringUser);
        current = current->next;
        free(stringUser);
    }
    return users;
}

void setPseudo(List *list, int idClient, char *pseudo){
    Client *current = list->head;
    while (current != NULL && current->id != idClient)
    {
        current = current->next;
    }
    if ((current != NULL) && (current->id == idClient))
    {
        current->pseudo = pseudo;
    }
}

Client* getClientById(List* list, int id){
    Client *current = list->head;
    while ((current != NULL) && (current->id != id))
    {
        current = current->next;
    }
    return current;
}

void changeACforJoin(List* list, int idClient)
{
    Client *current = list->head;
    while (current != NULL && current->id != idClient)
    {
        current = current->next;
    }
    if (current != NULL && current->alreadyConnected == 0)
    {
        current->alreadyConnected = 1;
    }
    else if (current != NULL && current->alreadyConnected == 1)
    {
        current->alreadyConnected = 0;
    }
}