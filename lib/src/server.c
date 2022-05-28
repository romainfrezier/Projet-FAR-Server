/**
 * @file server.c
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Server actions implementation
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "../headers/sema.h"
#include "../headers/server.h"
#include "../headers/admin.h"
#include "../headers/colors.h"
#include "../headers/commandServer.h"
#include "../headers/fileServer.h"
#include "../headers/channel.h"
#include "../headers/list.h"

/**
 * @brief List of all the server channels
 */
ChannelList *channelList;

/**
 * @brief Maximum number of channels
 */
int maxChannel = 5;

/**
 * @brief Maximum number of client per channel
 */
unsigned int MAX_CONNEXION = 5;

/**
 * @brief Default port of the server
 */
int defaultPort = 4000;

/**
 * @brief Number of channel in the server
 */
int channelCount = 0;

void launchServer()
{
  signal(SIGINT, serverQuit); // ^C
  signal(SIGTERM, serverQuit); // end of a terminal
  channelList = createChannelList(maxChannel);
  prepareGenerateChannel("Welcome", "The home channel !");
  // Server shutdown
  // free(adminKey);
  printf("End program\n");
}

void sendMessageForAllUsers(int signal)
{
  Channel *currentChannel = channelList->head;
  while (currentChannel != NULL)
  {
    if (currentChannel->thread == pthread_self()){ //check if the thread is the current channel thread
      Client *currentUser = currentChannel->clients->head;
      while (currentUser != NULL)
      {
        sendSpecificMessage(currentUser->id, allMessage);
        currentUser = currentUser->next;
      }
    }
    currentChannel = currentChannel->next;
  }
  greenMessage("Message transmitted to all\n");
}

void kickUserFromServer(int signal){
  Channel *currentChannel = channelList->head;
  while (currentChannel != NULL)
  {
    if (currentChannel->thread == pthread_self()){ //check if the thread is the current channel thread
      kick(kickedId, currentChannel->clients, currentChannel->semaphore, currentChannel->mutex);
    }
    currentChannel = currentChannel->next;
  }
}

void prepareGenerateChannel(char *name, char *theme)
{
  Channel *channelData = (Channel *)malloc(sizeof(Channel));
  channelData->port = (defaultPort + (channelCount * 5));
  channelData->name = name;
  channelData->channelList = channelList;
  channelData->theme = theme;
  pthread_t channelThread;
  pthread_create(&channelThread, NULL, generateChannel, channelData);
  channelData->thread = channelThread;
  channelCount += 1;
  pthread_join(channelThread, NULL);
}

void *generateChannel(void *channel)
{
  rk_sema sem;
  rk_sema_init(&sem, MAX_CONNEXION);
  pthread_mutex_t mutexList = PTHREAD_MUTEX_INITIALIZER;

  Channel *channelData = (Channel *)channel;
  Channel *channelCreated = createChannel(channelData->name, channelData->theme, channelData->port, channelData->thread, MAX_CONNEXION, sem, mutexList);
  addLastChannel(channelList, channelCreated);

  int port = (*channelCreated).port;

  printf("Start channel\n");

  int enable = 1;

  int dS = createAndBindSocket(port);
  int dSFileGet = createAndBindSocket(port + 1);
  int dSFileSend = createAndBindSocket(port + 2);

  listen(dS, MAX_CONNEXION);
  listen(dSFileGet, MAX_CONNEXION);
  listen(dSFileSend, MAX_CONNEXION);

  printf("Listening mode on port %d \n", port);
  greenMessage("The admin key is : ");
  greenMessage(adminKey);
  printf("\n");
  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);

  pthread_t fileGetThread;
  int *argGet = malloc(sizeof(int));
  *argGet = dSFileGet;
  pthread_create(&fileGetThread, NULL, fileGetThreadFunc, argGet);
  int *argSend = malloc(sizeof(int));
  *argSend = dSFileSend;
  pthread_t fileSendThread;
  pthread_create(&fileSendThread, NULL, fileSendThreadFunc, argSend);
  signal(SIGCHLD, sendMessageForAllUsers);
  signal(SIGUSR1, kickUserFromServer);
  while (1)
  {
    // Users are accepted until max allow
    // Wait for space in the user list
    rk_sema_wait(&sem);
    int acceptation = accept(dS, (struct sockaddr *)&aC, &lg);
    Client *current = (*channelCreated).clients->head;
    while ((current != NULL) && (current->alreadyConnected != 1))
    {
      current = current->next;
    }

    if (current == NULL)
    {
      sendSpecificMessage(acceptation, "Please choose your username : ");
    }
    else
    {
      current->id = acceptation;
    }

    changeACforJoin((*channelCreated).clients, acceptation);
    // and we attributed a reception thread to each
    tsr *receiveData = (tsr *)malloc(sizeof(tsr));
    (*receiveData).client = acceptation;
    (*receiveData).clients = (*channelCreated).clients;
    (*receiveData).currentChannel = getCurrentChannel(channelList);
    pthread_t receive;
    pthread_create(&receive, NULL, receiveMessage, (void *)receiveData);
  }
  // Shutdown of all user sockets
  Client *current = (*channelCreated).clients->head;
  do
  {
    delVal((*channelCreated).clients, current->id);
    shutdown(current->id, 2);
    current = current->next;
  } while (current->next != NULL);

  shutdown(dS, 2);
  rk_sema_destroy(&sem);
}

void *receiveMessage(void *sock_client)
{
  tsr *sock_cli = (tsr *)sock_client;
  Client *client = getClientById((*sock_cli).clients, (*sock_cli).client);
  if (client == NULL)
  {
    u_long sizePseudo;
    char *pseudo;
    int check;
    do
    {
      int receive = recv((*sock_cli).client, &sizePseudo, sizeof(u_long), 0);
      if (receive == 0)
      {
        return NULL;
      }
      else if (receive == -1){
        redErrorMessage("Error message size received\n");
      }
      blueMessage("Size received\n");
      pseudo = (char *)malloc(sizeof(char) * sizePseudo);
      receive = recv((*sock_cli).client, pseudo, sizePseudo, 0);
      if (receive == 0)
      {
        return NULL;
      }
      else if (receive == -1){
        redErrorMessage("Error message received\n");
      }
      blueMessage("Pseudo received : ");
      blueMessage(pseudo);
      printf("\n");
      check = pseudoInAllChannel(channelList, pseudo);
      if (check == 0)
      {
        sendSpecificMessage((*sock_cli).client, "Username already used !\nPlease choose another username : ");
      }
    } while (check == 0);

    sendSpecificMessage((*sock_cli).client, "Connected !");
    blueMessage("Pseudo accepted : ");
    blueMessage(pseudo);
    printf("\n");

    // add the client to the socket list
    pthread_mutex_lock(&(sock_cli->currentChannel->mutex));
    addFirst(sock_cli->clients, sock_cli->client, pseudo);
    pthread_mutex_unlock(&(sock_cli->currentChannel->mutex));
    printf("User connected with id : %d\n", sock_cli->client);
    // malloc() and free() are not thread-safe functions.
    // We need to protect the calls to those functions with a mutex.
    // This one is a problem
    //free(pseudo);
  }
  while (1)
  {
    u_long size;
    // Size reception
    int receive = recv((*sock_cli).client, &size, sizeof(u_long), 0);
    if (receive == 0)
    {
        redMessage("Connexion with user lost\n");
        break;
    }
    else if (receive == -1)
    {
      redErrorMessage("Error message size received\n");
    }

    blueMessage("Size received\n");

    // Message reception
    char *msg = (char *)malloc(size);
    if (recv((*sock_cli).client, msg, size, 0) <= 0)
    {
      redErrorMessage("Error message received\n");
    }
    blueMessage("Message received\n");

    int censorship = checkCensorship(msg);

    // check censorship before everything
    if (censorship != 0)
    {
      sendSpecificMessage((*sock_cli).client, "\033[0;31m\nDon't insult! Your message has not been sent...\n");
    }
    else if (msg[0] == '/')
    {
      // Commands management
      int checkCmd = checkCommand(msg, (*sock_cli).client, (*sock_cli).currentChannel, channelList);
      if (checkCmd == -1)
      {
        break;
      }
    }
    else
    {
      // Send to each user
      pthread_t send;
      Client *current = sock_cli->clients->head;
      displayList(sock_cli->clients);
      for (int i = 0; i < MAX_CONNEXION - sock_cli->clients->size; i++)
      {
        if (current->id != (*sock_cli).client)
        {
          tss *sendData = (tss *)malloc(sizeof(tss));
          (*sendData).client = current->id;
          (*sendData).size = size;
          (*sendData).message = msg;
          (*sendData).pseudoSender = getPseudoById(sock_cli->clients, (*sock_cli).client);

          // Execution of threads to send message to every one
          pthread_create(&send, NULL, transmitMessage, (void *)sendData);
        }
        current = current->next;
      }
    }
    // malloc() and free() are not thread-safe functions.
    // We need to protect the calls to those functions with a mutex.
    // This one is a problem
    //free(msg);
  }
  return NULL;
}

int createAndBindSocket(int port)
{
  int enable = 1;
  int dS = socket(PF_INET, SOCK_STREAM, 0);
  if (dS < 0)
  {
    redErrorMessage("ERROR opening socket fileSend");
  }
  if (setsockopt(dS, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed for fileSend");
  }
  struct sockaddr_in adFileSend;
  adFileSend.sin_family = AF_INET;
  adFileSend.sin_addr.s_addr = INADDR_ANY;
  adFileSend.sin_port = htons(port);
  bind(dS, (struct sockaddr *)&adFileSend, sizeof(adFileSend));
  return dS;
}

void serverQuit(int n)
{
  Channel *current = channelList->head;
  while (current != NULL)
  {
    channelQuit(current->clients, current->semaphore, current->mutex);
    redMessage("\nThe channel ");
    redMessage(current->name);
    redMessage(" has been stopped\n");
    current = current->next;
  }
  redErrorMessage("\nThe server has been stopped\n");
}