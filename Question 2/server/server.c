#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "lib/headers/stringFunc.h"
#include "lib/headers/list.h"
#include "lib/headers/server.h"
#include "lib/headers/colors.h"
#include "lib/headers/sema.h"
#include "lib/headers/fileServer.h"
#include "lib/headers/admin.h"
#include "lib/headers/commandServer.h"

unsigned int MAX_CONNECTION = 3;

struct rk_sema sem;
List *sockets;
pthread_mutex_t mutexList = PTHREAD_MUTEX_INITIALIZER;
int dSFileGet;
int dSFileSend;

// We want to create a send thread and a reception thread for each user
int main(int argc, char *argv[])
{
  // Definition of the socket array to the desired size
  sockets = createList(MAX_CONNECTION);
  rk_sema_init(&sem, MAX_CONNECTION);
  if (argc != 2)
  {
    redErrorMessage("Usage : ./exe port");
  }

  if (atoi(argv[1]) <= 1024)
  {
    redErrorMessage("Bad port: must be greater than 1024");
  }

  printf("Start program\n");

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
  ad.sin_port = htons(atoi(argv[1]));
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
  adFileGet.sin_port = htons(atoi(argv[1]) + 1);
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
  adFileSend.sin_port = htons(atoi(argv[1]) + 2);
  bind(dSFileSend, (struct sockaddr *)&adFileSend, sizeof(adFileSend));
  printf("File socketGet named\n");

  listen(dS, MAX_CONNECTION);
  listen(dSFileGet, MAX_CONNECTION);
  listen(dSFileSend, MAX_CONNECTION);

  printf("Listening mode on port %d \n", atoi(argv[1]));
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

  // Server shutdown
  free(adminKey);
  shutdown(dS, 2);
  rk_sema_destroy(&sem);
  printf("End program\n");
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
    check = pseudoInList(sockets, pseudo);
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
  pthread_mutex_lock(&mutexList);
  addFirst(sockets, sock_cli->client, pseudo);
  pthread_mutex_unlock(&mutexList);
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

    // Commands management here
    if (msg[0] == '/')
    {
      int checkCmd = checkCommand(msg, sock_cli);
      if (checkCmd == -1)
      {
        break;
      }
    }
    else
    {
      // Send to each user
      pthread_t send;
      Link *current = sockets->head;
      displayList(sockets);
      for (int i = 0; i < MAX_CONNECTION - sockets->size; i++)
      {
        if (current->value != (*sock_cli).client)
        {
          tss *sendData = (tss *)malloc(sizeof(tss));
          (*sendData).client = current->value;
          (*sendData).size = size;
          (*sendData).message = msg;
          (*sendData).pseudoSender = getPseudoById(sockets, (*sock_cli).client);

          // Execution of threads to send message to every one
          pthread_create(&send, NULL, transmitMessage, (void *)sendData);
        }
        current = current->next;
      }
    }
    // free(msg);
  }
}

// Sending a message to the client
void transmitMessage(void *sock_client)
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
}

// Allows sending a private message
void sendPrivateMessage(char *msg, int client)
{
  if (verifCommand(msg, 2) == 1)
  {
    char **mess = str_split(msg, 3);
    char *cmd = (char *)malloc(strlen(mess[0]));
    cmd = mess[0];
    char *target = mess[1];
    int id = getIdByPseudo(sockets, target);
    if (id != NULL)
    {
      char *pseudo = getPseudoById(sockets, client);
      char *pmPseudo = (char *)malloc(sizeof(char) * (strlen(pseudo) + strlen("(pm) ")));
      strcat(pmPseudo, "(pm) ");
      strcat(pmPseudo, pseudo);
      int commandSize = sizeof(cmd);
      int idSize = sizeof(target);
      tss *sendData = (tss *)malloc(sizeof(tss));
      (*sendData).client = id;
      (*sendData).size = strlen(mess[2]) + 1;
      (*sendData).message = mess[2];
      (*sendData).pseudoSender = pmPseudo;
      pthread_t send;
      pthread_create(&send, NULL, transmitMessage, (void *)sendData);
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

// Send a specific message to client selected
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
