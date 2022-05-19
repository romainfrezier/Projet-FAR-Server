#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "lib/headers/sema.h"
#include "lib/headers/server.h"
#include "lib/headers/admin.h"
#include "lib/headers/colors.h"
#include "lib/headers/commandServer.h"
#include "lib/headers/fileServer.h"
#include "lib/headers/chanel.h"
#include "lib/headers/list.h"

ChanelList *chanelList;
int maxChanel = 10;
unsigned int MAX_CONNECTION = 3;
int defaultPort = 4000;
int chanelCount = 0;

// We want to create a send thread and a reception thread for each user
int main(int argc, char *argv[])
{
  chanelList = createChanelList(maxChanel);
  prepareGenerateChanel("Welcome");
  // Server shutdown
  // free(adminKey);
  printf("End program\n");
}

void prepareGenerateChanel(char *name)
{
  ChanelStruct *chanelData = (ChanelStruct *)malloc(sizeof(ChanelStruct));
  chanelData->port = (defaultPort + (chanelCount * 5));
  chanelData->name = name;
  chanelData->chanelList = chanelList;
  pthread_t chanelThread;
  chanelData->thread = chanelThread;
  pthread_create(&chanelThread, NULL, generateChanel, chanelData);
  chanelCount += 1;
  pthread_join(chanelThread, NULL);
}

void generateChanel(void *chanel_struct)
{
  rk_sema sem;
  int dSFileGet;
  int dSFileSend;
  pthread_mutex_t mutexList = PTHREAD_MUTEX_INITIALIZER;
  ChanelStruct *chanelData = (ChanelStruct *)chanel_struct;
  Chanel *chanel = createChanel(chanelData->name, chanelData->port, chanelData->thread);
  addFirstChanel(chanelList, chanel);

  int port = (*chanelData).port;
  List *sockets = (*chanel).clients;
  // Definition of the socket array to the desired size
  sockets = createList(MAX_CONNECTION);
  rk_sema_init(&sem, MAX_CONNECTION);

  printf("Start chanel\n");

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
  printf("Socket created\n");

  struct sockaddr_in ad;
  ad.sin_family = AF_INET;
  ad.sin_addr.s_addr = INADDR_ANY;
  ad.sin_port = htons(port);
  bind(dS, (struct sockaddr *)&ad, sizeof(ad));
  printf("Socket named\n");

  dSFileGet = socket(PF_INET, SOCK_STREAM, 0);
  if (dSFileGet < 0)
  {
    redErrorMessage("ERROR opening socket fileGet");
  }
  if (setsockopt(dSFileGet, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed for fileGet");
  }
  printf("Socket fileGet created\n");

  struct sockaddr_in adFileGet;
  adFileGet.sin_family = AF_INET;
  adFileGet.sin_addr.s_addr = INADDR_ANY;
  adFileGet.sin_port = htons(port + 1);
  bind(dSFileGet, (struct sockaddr *)&adFileGet, sizeof(adFileGet));
  printf("File socketGet named\n");

  dSFileSend = socket(PF_INET, SOCK_STREAM, 0);
  if (dSFileSend < 0)
  {
    redErrorMessage("ERROR opening socket fileSend");
  }
  if (setsockopt(dSFileSend, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed for fileSend");
  }
  printf("Socket fileSend created\n");
  struct sockaddr_in adFileSend;
  adFileSend.sin_family = AF_INET;
  adFileSend.sin_addr.s_addr = INADDR_ANY;
  adFileSend.sin_port = htons(port + 2);
  bind(dSFileSend, (struct sockaddr *)&adFileSend, sizeof(adFileSend));
  printf("File socketGet named\n");

  listen(dS, MAX_CONNECTION);
  listen(dSFileGet, MAX_CONNECTION);
  listen(dSFileSend, MAX_CONNECTION);

  printf("Listening mode on port %d \n", port);
  greenMessage("The admin key is : ");
  greenMessage(adminKey);
  printf("\n");
  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);
  signal(SIGINT, serverQuit);

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
    sendSpecificMessage(acceptation, "Please choose your username : ");

    // and we attributed a reception thread to each
    tsr *receiveData = (tsr *)malloc(sizeof(tsr));
    (*receiveData).client = acceptation;
    (*receiveData).clients = sockets;
    (*receiveData).sem = sem;
    (*receiveData).mutex = mutexList;
    pthread_t receive;
    pthread_create(&receive, NULL, receiveMessage, (void *)receiveData);
  }
  // Shutdown of all user sockets
  Link *current = sockets->head;
  do
  {
    delVal(sockets, current->value);
    shutdown(current->value, 2);
    current = current->next;
  } while (current->next != NULL);

  shutdown(dS, 2);
  rk_sema_destroy(&sem);
}

// Reception of a client message
void receiveMessage(void *sock_client)
{
  tsr *sock_cli = (tsr *)sock_client;
  char *msg;
  u_long size;
  char *pseudo;
  int check;
  do
  {
    if (recv((*sock_cli).client, &size, sizeof(u_long), 0) == -1)
    {
      redErrorMessage("Error message size received\n");
    }
    blueMessage("Size received\n");
    pseudo = (char *)malloc(sizeof(char) * size);
    if (recv((*sock_cli).client, pseudo, size, 0) == -1)
    {
      redErrorMessage("Error message received\n");
    }
    blueMessage("Pseudo received : ");
    blueMessage(pseudo);
    printf("\n");
    check = pseudoInList(sock_cli->clients, pseudo);
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

  while (1)
  {
    // Size reception
    if (recv((*sock_cli).client, &size, sizeof(u_long), 0) == -1)
    {
      redErrorMessage("Error message size received\n");
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
      int checkCmd = checkCommand(msg, sock_cli, (*sock_cli).sem);
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
      for (int i = 0; i < MAX_CONNECTION - sock_cli->clients->size; i++)
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
}