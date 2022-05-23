#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "lib/headers/sema.h"
#include "lib/headers/server.h"
#include "lib/headers/admin.h"
#include "lib/headers/colors.h"
#include "lib/headers/commandServer.h"
#include "lib/headers/fileServer.h"
#include "lib/headers/channel.h"
#include "lib/headers/list.h"

ChannelList *channelList;
int maxChannel = 10;
unsigned int MAX_CONNEXION = 3;
int defaultPort = 4000;
int channelCount = 0;

// We want to create a send thread and a reception thread for each user
int main(int argc, char *argv[])
{
  channelList = createChannelList(maxChannel);
  prepareGenerateChannel("Welcome");
  // Server shutdown
  // free(adminKey);
  printf("End program\n");
}

void prepareGenerateChannel(char *name)
{
  Channel *channelData = (Channel *)malloc(sizeof(Channel));
  channelData->port = (defaultPort + (channelCount * 5));
  channelData->name = name;
  channelData->channelList = channelList;
  pthread_t channelThread;
  pthread_create(&channelThread, NULL, generateChannel, channelData);
  channelData->thread = channelThread;
  channelCount += 1;
  pthread_join(channelThread, NULL);
}

void * generateChannel(void *channel)
{
  rk_sema sem;
  int dSFileGet;
  int dSFileSend;
  pthread_mutex_t mutexList = PTHREAD_MUTEX_INITIALIZER;
  Channel *channelData = (Channel *)channel;
  Channel *channelCreated = createChannel(channelData->name, channelData->port, channelData->thread, MAX_CONNEXION);
  addLastChannel(channelList, channelCreated);

  int port = (*channelCreated).port;

  rk_sema_init(&sem, MAX_CONNEXION);

  printf("Start channel\n");

  int enable = 1;

  int dS = socket(PF_INET, SOCK_STREAM, 0);
  if (dS < 0)
  {
    redErrorMessage("ERROR opening socket");
  }
  if (setsockopt(dS, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed");
  }

  struct sockaddr_in ad;
  ad.sin_family = AF_INET;
  ad.sin_addr.s_addr = INADDR_ANY;
  ad.sin_port = htons(port);
  bind(dS, (struct sockaddr *)&ad, sizeof(ad));

  dSFileGet = socket(PF_INET, SOCK_STREAM, 0);
  if (dSFileGet < 0)
  {
    redErrorMessage("ERROR opening socket fileGet");
  }
  if (setsockopt(dSFileGet, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed for fileGet");
  }

  struct sockaddr_in adFileGet;
  adFileGet.sin_family = AF_INET;
  adFileGet.sin_addr.s_addr = INADDR_ANY;
  adFileGet.sin_port = htons(port + 1);
  bind(dSFileGet, (struct sockaddr *)&adFileGet, sizeof(adFileGet));

  dSFileSend = socket(PF_INET, SOCK_STREAM, 0);
  if (dSFileSend < 0)
  {
    redErrorMessage("ERROR opening socket fileSend");
  }
  if (setsockopt(dSFileSend, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed for fileSend");
  }

  struct sockaddr_in adFileSend;
  adFileSend.sin_family = AF_INET;
  adFileSend.sin_addr.s_addr = INADDR_ANY;
  adFileSend.sin_port = htons(port + 2);
  bind(dSFileSend, (struct sockaddr *)&adFileSend, sizeof(adFileSend));

  listen(dS, MAX_CONNEXION);
  listen(dSFileGet, MAX_CONNEXION);
  listen(dSFileSend, MAX_CONNEXION);

  printf("Listening mode on port %d \n", port);
  greenMessage("The admin key is : ");
  greenMessage(adminKey);
  printf("\n");
  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);
  signal(SIGINT, channelQuit);

  pthread_t fileGetThread;
  int *argGet = malloc(sizeof(int));
  *argGet = dSFileGet;
  pthread_create(&fileGetThread, NULL, fileGetThreadFunc, argGet);
  int *argSend = malloc(sizeof(int));
  *argSend = dSFileSend;
  pthread_t fileSendThread;
  pthread_create(&fileSendThread, NULL, fileSendThreadFunc, argSend);
  while (1)
  {
    // Users are accepted until max allow
    // Wait for space in the user list
    rk_sema_wait(&sem);
    int acceptation = accept(dS, (struct sockaddr *)&aC, &lg);
    Link *current = (*channelCreated).clients->head;
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
      current->value = acceptation;
    }

    changeACforJoin((*channelCreated).clients, acceptation);
    // and we attributed a reception thread to each
    tsr *receiveData = (tsr *)malloc(sizeof(tsr));
    (*receiveData).client = acceptation;
    (*receiveData).clients = (*channelCreated).clients;
    (*receiveData).sem = sem;
    (*receiveData).mutex = mutexList;
    pthread_t receive;
    pthread_create(&receive, NULL, receiveMessage, (void *)receiveData);
  }
  // Shutdown of all user sockets
  Link *current = (*channelCreated).clients->head;
  do
  {
    delVal((*channelCreated).clients, current->value);
    shutdown(current->value, 2);
    current = current->next;
  } while (current->next != NULL);

  shutdown(dS, 2);
  rk_sema_destroy(&sem);
}

// Reception of a client message
void * receiveMessage(void *sock_client)
{
  tsr *sock_cli = (tsr *)sock_client;
  printf("here %d\n",(*sock_cli).client);
  Link *client = getClientById((*sock_cli).clients, (*sock_cli).client);
  if (client == NULL)
  {
    u_long sizePseudo;
    char *pseudo;
    int check;
    do
    {
      if (recv((*sock_cli).client, &sizePseudo, sizeof(u_long), 0) == -1)
      {
        redErrorMessage("Error message size received\n");
      }
      blueMessage("Size received\n");
      pseudo = (char *)malloc(sizeof(char) * sizePseudo);
      if (recv((*sock_cli).client, pseudo, sizePseudo, 0) == -1)
      {
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
    pthread_mutex_lock(&(sock_cli->mutex));
    addFirst(sock_cli->clients, sock_cli->client, pseudo);
    pthread_mutex_unlock(&(sock_cli->mutex));
    printf("User connected with id : %d\n", sock_cli->client);
  }
  while (1)
  {
    u_long size;
    // Size reception
    int receive = recv((*sock_cli).client, &size, sizeof(u_long), 0);
    if (receive == -1)
    {
      redErrorMessage("Error message size received\n");
    }
    else if (receive == 0)
    {
      redMessage("Connexion with user lost\n");
      break;
    }

    blueMessage("Size received\n");

    // Message reception
    char *msg = (char *)malloc(size);
    if (recv((*sock_cli).client, msg, size, 0) == -1)
    {
      redErrorMessage("Error message received\n");
    }
    blueMessage("Message received\n");

    int censorship = checkCensorship(msg);

    if (censorship != 0)
    {
      sendSpecificMessage((*sock_cli).client, "\033[0;31m\nDon't insult! Your message has not been sent...\n");
    }
    else if (msg[0] == '/')
    {
      // Commands management here
      int checkCmd = checkCommand(msg, sock_cli, (*sock_cli).sem, channelList);
      if (checkCmd == -1)
      {
        break;
      }
    }
    else
    {
      // Send to each user
      pthread_t send;
      Link *current = sock_cli->clients->head;
      displayList(sock_cli->clients);
      for (int i = 0; i < MAX_CONNEXION - sock_cli->clients->size; i++)
      {
        if (current->value != (*sock_cli).client)
        {
          tss *sendData = (tss *)malloc(sizeof(tss));
          (*sendData).client = current->value;
          (*sendData).size = size;
          (*sendData).message = msg;
          (*sendData).pseudoSender = getPseudoById(sock_cli->clients, (*sock_cli).client);

          // Execution of threads to send message to every one
          pthread_create(&send, NULL, transmitMessage, (void *)sendData);
        }
        current = current->next;
      }
    }
    // free(msg);
  }
  return NULL;
}