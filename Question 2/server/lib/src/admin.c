#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../headers/admin.h"
#include "../headers/list.h"
#include "../headers/chanel.h"
#include "../headers/stringFunc.h"
#include "../headers/commandServer.h"

char* adminKey = "1234";

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
void kick(char *message, int client, List* sockets)
{
    if (countSpaceCommand(message, 1) == 1)
    {
        if (isUserAdmin(sockets, client) == 1)
        {
            char **mess = str_split(message, 2);
            int idKickedClient = getIdByPseudo(sockets, mess[1]);
            if (idKickedClient != -1)
            {
                sendSpecificMessage(idKickedClient, "You have been kicked by an admin !\n");
                printf("User %d has been kicked by admin %d \n", idKickedClient, client);
                sendSpecificMessage(idKickedClient, "/quit");
                sendSpecificMessage(client, "The user has been kicked !\n");
            }
            else
            {
                sendSpecificMessage(client, "The user doesn't exist !\n");
            }
        }
        else
        {
            sendSpecificMessage(client, "You don't have the permission !\n");
        }
    }
    else
    {
        sendSpecificMessage(client, "The command is : [/kick targetPseudo] \n");
    }
}

// check the password give by a user
void adminVerification(char *message, int client, List* sockets)
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
                sendSpecificMessage(client, "You are now an admin !\n");
            }
        }
        else
        {
            sendSpecificMessage(client, "That's not the current admin key !\n");
        }
    }
    else
    {
        sendSpecificMessage(client, "The command is : [/admin adminKey] \n");
    }
}