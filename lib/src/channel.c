#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "../headers/channel.h"
#include "../headers/sema.h"
#include "../headers/commandServer.h"
#include "../headers/stringFunc.h"
#include "../headers/tools.h"

Channel *createChannel(char *name, char *theme, int port, pthread_t thread, int size, rk_sema semaphore, pthread_mutex_t mutexList)
{
    Channel *newChannel = (Channel *)malloc(sizeof(Channel));
    List *sockets = createList(size);
    newChannel->name = (char *)malloc(100);
    newChannel->name = name;
    newChannel->theme = (char *)malloc(100);
    newChannel->theme = theme;
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
    char *arr[3];
    getRegexGroup(arr, 3, cmd, "/channel +([^ ]+) +(.+) *$");
    // char **msg = str_split(cmd, 3);
    prepareGenerateChannel(arr[1], arr[2]);
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
    char *arrOne[4];
    char *arrTwo[4];
    char *arrThree[5];
    getRegexGroup(arrOne, 4, message, "^/mchannel +-(n) +([0-9]{1}) +([^ ].*) *$");
    getRegexGroup(arrTwo, 4, message, "^/mchannel +-(t) +([0-9]{1}) +(.*) *$");
    getRegexGroup(arrThree, 5, message, "^/mchannel +-(nt|tn) +([0-9]{1}) +([^ ]+) +([^ ].*) *$");

    if (isUserAdmin(clients, client) == 1)
    {
        if (strcmp(arrOne[1], "n") == 0)
        {
            int index = atoi(arrOne[2]);
            if (index == 1){
                sendSpecificMessage(client, "\nYou can not modify the channel number 1\n");
            }
            else
            {
                Channel *chosenChannel = getChannelByIndex(channelList, index);
                chosenChannel->theme = arrOne[3];
            }
        }
        else if (strcmp(arrTwo[1], "t") == 0)
        {
            int index = atoi(arrTwo[2]);
            if (index == 1){
                sendSpecificMessage(client, "\nYou can not modify the channel number 1\n");
            }
            else
            {
                Channel *chosenChannel = getChannelByIndex(channelList, index);
                chosenChannel->name = arrTwo[3];
            }
        }
        else if (strcmp(arrThree[1], "tn") == 0 || strcmp(arrThree[1], "nt") == 0)
        {
            int index = atoi(arrThree[2]);
            if (index == 1){
                sendSpecificMessage(client, "\nYou can not modify the channel number 1\n");
            }
            else
            {
                Channel *chosenChannel = getChannelByIndex(channelList, index);
                chosenChannel->name = arrThree[3];
                chosenChannel->theme = arrThree[4];
            }
        }
        else
        {
            sendSpecificMessage(client, "\nThe command is /mchannel [-n][-t] number content\n");
        }
        // free(arrOne);
        // free(arrTwo);
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
            int resRegex = regex(message, "/channel +([^ ]+) +(.+) *$");
            if (resRegex == 0)
            {
                pthread_t createChannelThread;
                pthread_create(&createChannelThread, NULL, createNewChannel, message);
            }
            else
            {
                sendSpecificMessage(client, "\nThe command is /channel channelName channelTheme !\n");
            }
        }
    }
    else
    {
        sendSpecificMessage(client, "\nYou can't create a channel if your are not an admin\n");
    }
}

void removeChannel(char *msg, ChannelList *channelList, int client, List *clients)
{
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
        channelQuit(chosenChannel->clients, chosenChannel->semaphore, chosenChannel->mutex);
        delChannel(channelList, index);
    }
}