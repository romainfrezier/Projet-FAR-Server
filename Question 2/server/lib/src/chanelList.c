#include <stdio.h>
#include <stdlib.h>
#include "chanelList.h"
#include "List.h"

ChanelList* createChanelList(int size){
    ChanelList* newChanelList = (ChanelList*)malloc(sizeof(ChanelList));
    newChanelList->size = size;
    newChanelList->head = NULL;
    return newChanelList;
}


Chanel* createChanel(char* name, int port){
    Chanel* newChanel = (Chanel*)malloc(sizeof(Chanel));
    List* sockets = (List*)malloc(sizeof(List));
    newChanel->name = name;
    newChanel->port = port;
    newChanel->clients = sockets;
    return newChanel;
}

// check if the list is empty. Return 0 if the list is empty
int chanelListIsEmpty(ChanelList* list)
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
void addFirst(ChanelList* list, Chanel* chanel)
{
    if (list->size == 0)
    {
        printf("There is no more space in the chanel list !\n");
    }
    else
    {
        if (list->head == NULL)
        {
            list->head = chanel;
            chanel->next = NULL;
        }
        else
        {
            chanel->next = list->head;
            list->head = chanel;
        }
        list->size = list->size - 1;
    }
}
