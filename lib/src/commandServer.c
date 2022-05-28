/**
 * @file commandServer.c
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Command management functions implementation
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <regex.h>

#include "../headers/channel.h"
#include "../headers/admin.h"
#include "../headers/server.h"
#include "../headers/sema.h"
#include "../headers/commandServer.h"
#include "../headers/colors.h"
#include "../headers/fileServer.h"
#include "../headers/stringFunc.h"
#include "../headers/tools.h"

void sendSpecificMessage(int client, char *message)
{
    u_long sizeMessage = strlen(message) + 1;
    // Send connection message size
    if (send(client, &sizeMessage, sizeof(u_long), 0) == -1)
    {
        redErrorMessage("Error sending size\n");
    }
    // Send connection message
    if (send(client, message, sizeMessage, 0) == -1)
    {
        redErrorMessage("Error sending connection message\n");
    }
}

int checkCommand(char *msg, int client, Channel *channel, ChannelList *channelList)
{
    Channel *currentChannel = channel;
    printf("Command detected\n");
    char *copyMessage = (char *)malloc(strlen(msg) + 1);
    strcpy(copyMessage, msg);
    char *strto = strtok(copyMessage, " ");
    if (strcmp(msg, "/quit") == 0)
    {
        userQuit(client, currentChannel->clients, currentChannel->semaphore, currentChannel->mutex);
        return -1;
    }
    else if (strcmp(strto, "/pm") == 0)
    {
        printf("Go to private message\n");
        sendPrivateMessage(msg, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/admin") == 0)
    {
        printf("Go to admin verification\n");
        adminVerification(msg, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/kick") == 0)
    {
        printf("Go to kick function\n");
        prepareKick(channelList, msg, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/luser") == 0)
    {
        printf("Go to getAllChannelUsers function \n");
        sendSpecificMessage(client, getAllChannelUsers(currentChannel->clients, client, "\nUsers connected in this channel : \n\n"));
    }
    else if (strcmp(strto, "/luser-all") == 0)
    {
        printf("Go to displayAllUsers function \n");
        displayAllUsers(channelList, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/rename") == 0)
    {
        printf("Go to rename function \n");
        renameUser(msg, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/lfile") == 0)
    {
        printf("Go to list file function \n");
        sendSpecificMessage(client, listFile("./serverStorage/"));
    }
    else if (strcmp(strto, "/lchannel") == 0)
    {
        printf("Go to list channel function \n");

        sendSpecificMessage(client, listChannel(channelList, client));
    }
    else if (strcmp(strto, "/cchannel") == 0)
    {
        printf("Go to create channel function\n");
        checkChannel(currentChannel->clients, client, channelList->freePlaces, msg);
    }
    else if (strcmp(strto, "/jchannel") == 0)
    {
        printf("Go to join channel function\n");
        joinChannel(msg, channelList, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/mchannel") == 0)
    {
        printf("Go to modify channel function\n");
        modifyChannel(channelList, msg, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/rmchannel") == 0)
    {
        printf("Go to remove channel function\n");
        removeChannel(msg, channelList, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/addword") == 0)
    {
        printf("Go to add censorship words function\n");
        addWord(msg, currentChannel->clients, client);
        //        removeChannel(msg, channelList, (*sock_cli).client, (*sock_cli).clients);
    }
    else if (strcmp(strto, "/all") == 0)
    {
        printf("Go to all sendAllMessage function \n");
        sendAllMessage(msg, channelList, currentChannel->clients, client);
    }
    else if (strcmp(strto, "/shutdown") == 0)
    {
        printf("Go to shutdown function\n");
        shutdownServer(channelList, currentChannel->clients, client);
    }
    else if (strcmp(strto, "/cform") == 0)
    {
        printf("Go to create form function\n");
        prepareAddFormFirst(currentChannel->formList, msg, client, currentChannel->clients);
    }
    else if (strcmp(strto, "/aform") == 0)
    {
        printf("Go to anwser form function\n");
        prepareAnswer(msg, client, currentChannel->formList, currentChannel->clients);

    }
    else if (strcmp(strto, "/rmform") == 0)
    {
        printf("Go to anwser delete form function\n");
        prepareRemoveByIndex(msg, currentChannel->formList, client, currentChannel->clients);
        
    }
    else if (strcmp(strto, "/lform") == 0)
    {
        printf("Go to list form function\n");
        sendSpecificMessage(client, listForm(currentChannel->formList));
    }
    else if (strcmp(strto, "/sform") == 0)
    {
        printf("Go to see form function\n");
        prepareSeeResults(client, msg, currentChannel->formList);
    }
    else
    {
        sendSpecificMessage(client, "\nThis is an unknown command !\n");
    }
    return 0;
}

void displayAllUsers(ChannelList *list, int client, List *clients)
{
    char *listToSend = getAllUsers(list, client, clients);
    if (listToSend == NULL)
    {
        sendSpecificMessage(client, "\n\033[0;31mYou are not an admin ! \n");
    }
    else
    {
        sendSpecificMessage(client, listToSend);
    }
}

void sendAllMessage(char *msg, ChannelList *channelList, List *clients, int client)
{
    if (isUserAdmin(clients, client) == 1)
    {
        sendAllUsersMessage(channelList, msg);
    }
    else
    {
        sendSpecificMessage(client, "\n\033[0;31mYou are not an admin ! \n");
    }
}

void shutdownServer(ChannelList *channelList, List *clients, int client)
{
    if (isUserAdmin(clients, client) == 1)
    {
        printf("Go to all sendAllMessage function \n");
        sendAllMessage("/all An admin decide to shutdown the server", channelList, clients, client);
        printf("Go to all shutdown function \n");
        sleep(1);
        serverQuit(2);
    }
    else
    {
        sendSpecificMessage(client, "\n\033[0;31mYou are not an admin ! \n");
    }
}

void userQuit(int socket, List *sockets, rk_sema sem, pthread_mutex_t mutexList)
{
    pthread_mutex_lock(&mutexList);
    delVal(sockets, socket);
    pthread_mutex_unlock(&mutexList);
    shutdown(socket, 2);
    rk_sema_post(&sem);
    printf("\t• User %d has been stopped\n", socket);
}

int checkCensorship(char *message)
{
    FILE *fp = fopen("lib/censorship_words.txt", "r");
    if (fp == NULL)
    {
        redErrorMessage("Error opening censor file...\n");
    }
    char *start = "^.*(";
    char *model = (char *)malloc(strlen(start));
    strcat(model, start);
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1)
    {
        char *word = strtok(line, "\n");
        model = realloc(model, strlen(model) + strlen(word) + strlen("(|)"));
        strcat(model, "(");
        strcat(model, word);
        strcat(model, ")|");
    }
    model[strlen(model) - 1] = ')';
    char *end = ".*$";
    model = realloc(model, strlen(model) + strlen(end));
    strcat(model, end);
    fclose(fp);
    free(line);

    int regexRes = regex(message, model);
    //    free(model);
    if (regexRes == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void renameUser(char *msg, int client, List *sockets)
{
    char **mess = str_split(msg, 2);
    char *newPseudo = mess[1];
    if (pseudoInList(sockets, newPseudo) == 0)
    {
        sendSpecificMessage(client, "Username already used !\nPlease choose another username");
    }
    else
    {
        setPseudo(sockets, client, newPseudo);
    }
}

void sendPrivateMessage(char *msg, int client, List *sockets)
{
    if (countSpaceCommand(msg, 2) == 1)
    {
        char **mess = str_split(msg, 3);
        char *target = mess[1];
        int id = getIdByPseudo(sockets, target);
        if (id != -1)
        {
            char *pseudo = getPseudoById(sockets, client);
            char *pmPseudo = (char *)malloc(sizeof(char) * (strlen(pseudo) + strlen("(pm) ")));
            strcat(pmPseudo, "(pm) ");
            strcat(pmPseudo, pseudo);
            tss *sendData = (tss *)malloc(sizeof(tss));
            (*sendData).client = id;
            (*sendData).size = strlen(mess[2]) + 1;
            (*sendData).message = mess[2];
            (*sendData).pseudoSender = pmPseudo;
            pthread_t send;
            pthread_create(&send, NULL, transmitMessage, (void *)sendData);
            //            free(pmPseudo);
        }
        else
        {
            sendSpecificMessage(client, "This user doesn't exist !");
            printf("The user doesn't exist ! \n");
        }
    }
    else
    {
        sendSpecificMessage(client, "The command is : [/pm targetPseudo yourMessage] \n");
    }
}

void *transmitMessage(void *sock_client)
{
    tss *sock_cli = (tss *)sock_client;
    u_long pseudoSize = strlen((*sock_cli).pseudoSender) + 1;
    struct rk_sema semSend;
    rk_sema_init(&semSend, 1);

    // strcat
    u_long globalSize = pseudoSize + (*sock_cli).size + 1;
    char *messageTransmited = (char *)malloc(globalSize * sizeof(char));
    strcat(messageTransmited, (*sock_cli).pseudoSender);
    strcat(messageTransmited, " : ");
    strcat(messageTransmited, (*sock_cli).message);

    // Transmit message size
    sendSpecificMessage((*sock_cli).client, messageTransmited);
    greenMessage("Message transmitted\n");
    //    free(messageTransmited);
    return NULL;
}

void sendSpecificNumber(int client, int number)
{
    int numberToSend = number;
    // Send connection message size
    if (send(client, &numberToSend, sizeof(int), 0) == -1)
    {
        redErrorMessage("Error sending number\n");
    }
}

void addWord(char *message, List *clients, int client)
{
    if (isUserAdmin(clients, client) == 1)
    {
        char *words[2];
        getRegexGroup(words, 2, message, "^/addword +([^ ]+) *$");
        char *wordToAdd = (char *)malloc(strlen(words[1]) + 2);
        strcpy(wordToAdd, "\n");
        strcat(wordToAdd, words[1]);
        FILE *fp;
        fp = fopen("lib/censorship_words.txt", "a");
        fprintf(fp, "%s", wordToAdd);
        fclose(fp);
        //        free(wordToAdd);
    }
    else
    {
        sendSpecificMessage(client, "\n\033[0;31mYou are not an admin ! \n");
    }
}