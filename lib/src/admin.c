/**
 * @file admin.c
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Admin functions implementation
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "../headers/channel.h"
#include "../headers/admin.h"
#include "../headers/stringFunc.h"
#include "../headers/commandServer.h"
#include "../headers/tools.h"

/**
 * @brief Default admin password
 */
char *adminKey = "1234";

/**
 * @brief Message send to all the channels (maximum size 100)
 */
char allMessage[100];

/**
 * @brief id of kicked user
 */
int kickedId;

/**
 * @brief Mutex on all message
 */
pthread_mutex_t allMessageMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Mutex on kick user
 */
pthread_mutex_t kickedIdMutex = PTHREAD_MUTEX_INITIALIZER;


char *generateAdminKey()
{
    char *key = (char *)malloc(10);
    for (int i = 0; i < 10; i++)
    {
        char randomletter = 'A' + (rand() % 26);
        strcat(key, &randomletter);
    }
    return key;
}

void prepareKick(ChannelList *channels, char *message, int client, List *sockets)
{
    if (regex(message, "^\\/kick +([a-zA-Z0-9\\.-]+) *$") == 0)
    {
        if (isUserAdmin(sockets, client) == 1)
        {
            char *mess[2];
            getRegexGroup(mess, 2, message, "^\\/kick +([a-zA-Z0-9\\.-]+) *$");
            pthread_mutex_lock(&kickedIdMutex);
            Channel *currentChannel = channels->head;
            int isFound = 0;
            while (currentChannel != NULL && isFound == 0)
            {
                kickedId = getIdByPseudo(currentChannel->clients, mess[1]);
                if (kickedId != -1)
                {
                    pthread_kill(currentChannel->thread, SIGUSR1);
                    isFound = 1;
                }
                currentChannel = currentChannel->next;
            }
            if (isFound == 0)
            {
                sendSpecificMessage(client, "The user doesn't exist !");
            }
            pthread_mutex_unlock(&kickedIdMutex);
        }
        else
        {
            sendSpecificMessage(client, "\n\033[0;31mYou are not an admin ! \n");
        }
    }
    else
    {
        sendSpecificMessage(client, "\nThe command is : [/kick targetPseudo]");
    }
}

void kick(int client, List *sockets, rk_sema semaphore, pthread_mutex_t mutex)
{
    if (client != -1)
    {
        printf("User %d has been kicked by an admin\n", client);
        sendSpecificMessage(client, "\n\033[0;31mYou have been by kick by an admin\n");
        userQuit(client, sockets, semaphore, mutex);
    }
}

void sendAllUsersMessage(ChannelList *channels, char *message)
{
    pthread_mutex_lock(&allMessageMutex);
    strcpy(allMessage, "");
    char *msg[2];
    getRegexGroup(msg, 2, message, "^/all *(.*)$");
    strcat(allMessage, "(ALL) ");
    strcat(allMessage, msg[1]);
    pthread_mutex_unlock(&allMessageMutex);
    Channel *currentChannel = channels->head;
    while (currentChannel != NULL)
    {
        pthread_kill(currentChannel->thread, SIGCHLD);
        currentChannel = currentChannel->next;
    }
}

void adminVerification(char *message, int client, List *sockets)
{
    if (regex(message, "^\\/admin +([a-zA-Z0-9]+) *$") == 0)
    {
        char *mess[2];
        getRegexGroup(mess, 2, message, "^\\/admin +([a-zA-Z0-9]+) *$");
        if (strcmp(adminKey, mess[1]) == 0)
        {
            char *pseudo = getPseudoById(sockets, client);
            if (pseudo != NULL && isUserAdmin(sockets, client) == -1)
            {
                setUserAdmin(sockets, client);
                sendSpecificMessage(client, "\nYou are now an admin !\n");
            }
            else
            {
                sendSpecificMessage(client, "\nYou already are an admin !\n");
            }
        }
        else
        {
            sendSpecificMessage(client, "\nThat's not the current admin key !\n");
        }
    }
    else
    {
        sendSpecificMessage(client, "\nThe command is : [/admin adminKey]\n");
    }
}