#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/admin.h"
#include "../headers/list.h"
#include "../headers/server.h"
#include "../headers/stringFunc.h"

char* adminKey = "1234";

void generateAdminKey(char *key)
{
    for (int i = 0; i < 10; i++)
    {
        char randomletter = 'A' + (rand() % 26);
        strcat(key, &randomletter);
    }
}

void kick(char *message, int client)
{
    if (verifCommand(message, 1) == 1)
    {
        if (isUserAdmin(sockets, client) == 1)
        {
            char **mess = str_split(message, 2);
            int idKickedClient = getIdByPseudo(sockets, mess[1]);
            if (idKickedClient != NULL)
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

void adminVerification(char *message, int client)
{
    if (verifCommand(message, 1) == 1)
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