#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "../headers/admin.h"
#include "../headers/stringFunc.h"
#include "../headers/commandServer.h"
#include "../headers/tools.h"


char *adminKey = "1234";
char allMessage[100];

// generate a random admin key
void generateAdminKey(char *key)
{
    for (int i = 0; i < 10; i++)
    {
        char randomletter = 'A' + (rand() % 26);
        strcat(key, &randomletter);
    }
}

// remove a user from the chat server
void kick(char *message, int client, List *sockets, rk_sema semaphore, pthread_mutex_t mutex)
{
    if (countSpaceCommand(message, 1) == 1)
    {
        if (isUserAdmin(sockets, client) == 1)
        {
            char **mess = str_split(message, 2);
            int idKickedClient = getIdByPseudo(sockets, mess[1]);
            if (idKickedClient != -1)
            {
                sendSpecificMessage(idKickedClient, "\n\033[0;31mYou are not an admin ! \n");
                printf("User %d has been kicked by admin %d \n", idKickedClient, client);
                userQuit(idKickedClient, sockets,semaphore,mutex);
            }
            else
            {
                sendSpecificMessage(client, "The user doesn't exist !");
            }
        }
        else
        {
            sendSpecificMessage(client, "You don't have the permission !");
        }
    }
    else
    {
        sendSpecificMessage(client, "The command is : [/kick targetPseudo]");
    }
}

void sendAllUsersMessage(ChannelList *channels, char *message)
{
    strcpy(allMessage, "");
    char* msg[2];
    getRegexGroup(msg, 2, message, "^/all *(.*)$");
    strcat(allMessage, "(ALL) ");
    strcat(allMessage, msg[1]);
    Channel *currentChannel = channels->head;
    while (currentChannel != NULL)
    {
        pthread_kill(currentChannel->thread, SIGCHLD);
        currentChannel = currentChannel->next;
    }
}

// check the password give by a user
void adminVerification(char *message, int client, List *sockets)
{
    if (countSpaceCommand(message, 1) == 1)
    {
        char **mess = str_split(message, 2);
        if (strcmp(adminKey, mess[1]) == 0)
        {
            char *pseudo = getPseudoById(sockets, client);
            if (pseudo != NULL)
            {
                setUserAdmin(sockets, client);
                sendSpecificMessage(client, "\nYou are now an admin !\n");
            }
        }
        else
        {
            sendSpecificMessage(client, "That's not the current admin key !");
        }
    }
    else
    {
        sendSpecificMessage(client, "The command is : [/admin adminKey] \n");
    }
}