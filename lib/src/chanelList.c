#include <stdio.h>
#include <stdlib.h>

#include "../headers/chanel.h"
#include "../headers/list.h"

ChanelList* createChanelList(int size){
    ChanelList* newChanelList = (ChanelList*)malloc(sizeof(ChanelList));
    newChanelList->size = size;
    newChanelList->head = NULL;
    return newChanelList;
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
void addFirstChanel(ChanelList* list, Chanel* chanel)
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


