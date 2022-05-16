#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../headers/list.h"
#include "../headers/server.h"
#include "../headers/admin.h"
#include "../headers/sema.h"
#include "../headers/colors.h"
#include "../headers/fileServer.h"
#include "../headers/commandServer.h"

// check wich command the user give
int checkCommand(char *msg, tsr *sock_cli)
{
    printf("Command detected\n");
    char *copyMessage = (char *)malloc(strlen(msg) + 1);
    strcpy(copyMessage, msg);
    char *strto = strtok(copyMessage, " ");
    if (strcmp(msg, "/quit") == 0)
    {
        userQuit((*sock_cli).client);
        return -1;
    }
    else if (strcmp(strto, "/pm") == 0)
    {
        printf("Go to private message\n");
        sendPrivateMessage(msg, (*sock_cli).client);
    }
    else if (strcmp(strto, "/admin") == 0)
    {
        printf("Go to admin verification\n");
        adminVerification(msg, (*sock_cli).client);
    }
    else if (strcmp(strto, "/kick") == 0)
    {
        printf("Go to kick function\n");
        kick(msg, (*sock_cli).client);
    }
    else if (strcmp(strto, "/users") == 0)
    {
        printf("Go to displayUsers function \n");
        displayAllUsers((*sock_cli).client);
    }
    else if (strcmp(strto, "/files") == 0)
    {
        printf("Go to list file function \n");
        char *list = listFile("./serverStorage");
        sendSpecificMessage((*sock_cli).client, list);
    }
    return 0;
}

// Allows the server to stop and stop all the user connected
void serverQuit(int n)
{
    // Shutdown of all user sockets
    Link *current = sockets->head;
    while (current != NULL)
    {
        Link *next = current->next;
        userQuit(current->value);
        current = next;
    }
    redErrorMessage("\nThe server has been stopped !\n");
    exit(0);
}

// send a message of all user of the chat server
void displayAllUsers(int client)
{
    sendSpecificMessage(client, getAllUsers(sockets, client));
}

// Allows a user to leave the server
void userQuit(int socket)
{
    pthread_mutex_lock(&mutexList);
    delVal(sockets, socket);
    pthread_mutex_unlock(&mutexList);
    shutdown(socket, 2);
    rk_sema_post(&sem);
    printf("User %d has been stopped\n", socket);
}