#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/channel.h"

ChannelList* createChannelList(int size){
    ChannelList* newChannelList = (ChannelList*)malloc(sizeof(ChannelList));
    newChannelList->freePlaces = size;
    newChannelList->head = NULL;
    newChannelList->maxPlaces = size;
    return newChannelList;
}


// check if the list is empty. Return 0 if the list is empty
int channelListIsEmpty(ChannelList* list)
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

// add a user to the last position of the list
void addLastChannel(ChannelList* list, Channel* channel)
{
    if (list->freePlaces == 0)
    {
        printf("There is no more space in the channel list !\n");
    }
    else
    {
        if (list->head == NULL)
        {
            list->head = channel;
            channel->next = NULL;
        }
        else
        {
            Channel *current = list->head;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = channel;
        }
        list->freePlaces = list->freePlaces - 1;
    }
}

char* listChannel(ChannelList* list){
    Channel *current = list->head;

    char *start = "List of channel(s) : \n";
    char *finalString = (char *)malloc(strlen(start));
    strcat(finalString, start);
    int count = 1;

    while (current != NULL)
    {
        char *line = (char *)malloc(strlen(current->name) + strlen("    00. \n"));
        char* number;
        asprintf(&number, "%d", count);
        strcat(line, "   ");
        strcat(line, number);
        strcat(line, ". ");
        strcat(line, current->name);
        strcat(line, "\n");
        finalString = realloc(finalString, strlen(finalString) + strlen(line));
        strcat(finalString, line);
        free(line);
        current = current->next;
        count ++;
    }

    return finalString;
}

Channel* getChannelByIndex(ChannelList* list, int index){
    Channel *current = list->head;
    int index_current = 1;
    if (index == 1){
        printf("ouiii \n");
        return current;
    }
    else {
        if (index <= list->maxPlaces){
            while (index_current != index)
            {
                current = current->next;
                index_current++;
            }
            return current;
        }
        else
        {
            return NULL;
        }
    }

}

int pseudoInAllChannel(ChannelList* list, char* pseudo){
    Channel *currentChannel = list->head;
    int res = 1;
    while (currentChannel != NULL && res != 0)
    {
        res = pseudoInList(currentChannel->clients, pseudo);
        currentChannel = currentChannel->next;
    }
    return res;
}