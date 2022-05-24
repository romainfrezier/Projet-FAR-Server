#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../headers/channel.h"
#include "../headers/sema.h"
#include "../headers/commandServer.h"
#include "../headers/stringFunc.h"
#include "../headers/colors.h"

Channel *createChannel(char *name, int port, pthread_t thread, int size, rk_sema semaphore, pthread_mutex_t mutexList)
{
    Channel* newChannel = (Channel*)malloc(sizeof(Channel));
    List* sockets = createList(size);
    newChannel->name = name;
    newChannel->port = port;
    newChannel->clients = sockets;
    newChannel->thread = thread;
    newChannel->next = NULL;
    newChannel->semaphore = semaphore;
    newChannel->mutex = mutexList;
    return newChannel;
}

void *createNewChannel(void *cmd)
{
    if (countSpaceCommand(cmd, 1) == 1)
    {
        char **msg = str_split(cmd, 1);
        prepareGenerateChannel(msg[1]);
    }
    else
    {
        printf("/channel channelName ! \n");
    }
    return NULL;
}

// Allows the server to stop and stop all the user connected
void channelQuit(List *sockets, rk_sema sem, pthread_mutex_t mutexList)
{
    // Shutdown of all user sockets
    Link *current = sockets->head;
    printf("\n");
    while (current != NULL)
    {
        userQuit(current->value, sockets, sem, mutexList);
        current = current->next;
    }
}

void modifyChannel(ChannelList *channelList, char *message, int client, List *clients)
{
    if (isUserAdmin(clients, client) == 1)
    {
        char **array = str_split(message, 3);
        int index = atoi(array[1]);
        Channel *chosenChannel = getChannelByIndex(channelList, index);
        chosenChannel->name = array[2];
    }
    else
    {
        sendSpecificMessage(client, "\nYou can't modify a channel if your are not an admin\n");
    }
}
void joinChannel(char *msg, ChannelList *channelList, int idClient, List *clients)
{
    char **cmd = str_split(msg, 1);
    int index = atoi(cmd[1]);
    Channel *chosenChannel = getChannelByIndex(channelList, index);

    int port = chosenChannel->port;
    char *portString = (char *)malloc(4 * sizeof(char));
    sprintf(portString, "%d", port);

    char *sendCommand = (char *)malloc(strlen(portString) + strlen(cmd[0]) + 1);
    strcpy(sendCommand, cmd[0]);
    strcat(sendCommand, " ");
    strcat(sendCommand, portString);

    // We add to the client to the good channel
    changeACforJoin(clients, idClient);
    Link *client = getClientById(clients, idClient);
    char *clientPseudo = getPseudoById(clients, idClient);
    addFirstClient(chosenChannel->clients, client, clientPseudo);

    // We delete from this channel the good client
    delVal(clients, idClient);

    // We send the port to the client
    sendSpecificMessage(idClient, sendCommand);
}

void checkChannel(List *clients, int client, int freePlaces, char *message)
{
    if (isUserAdmin(clients, client) == 1)
    {
        if (freePlaces == 0)
        {
            sendSpecificMessage(client, "\nThe maximum number of channels has been reached.\nYou can no longer add it for the moment.\n");
        }
        else
        {
            pthread_t createChannelThread;
            pthread_create(&createChannelThread, NULL, createNewChannel, message);
        }
    }
    else
    {
        sendSpecificMessage(client, "\nYou can't create a channel if your are not an admin\n");
    }
}

void removeChannel(char *msg, ChannelList *channelList, int client, List *clients){
    char **cmd = str_split(msg, 1);
    int index = atoi(cmd[1]);
    if (index == 1)
    {
        sendSpecificMessage(client, "\nYou can not remove the first channel\n");
    }
    else
    {
        Channel *chosenChannel = getChannelByIndex(channelList, index);
        Link *current = chosenChannel->clients->head;
        while (current != NULL)
        {
            sendSpecificMessage(current->value, "The channel where you are will be removed.\nYou will be ejecting from the server");
            current = current->next;
        }
        sleep(1);
        channelQuit(chosenChannel->clients, chosenChannel->semaphore, chosenChannel->mutex);
        delChannel(channelList, index);
    }
}