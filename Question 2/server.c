#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "stringFunc.h"
#include "list.h"
#include "server.h"
#include "colors.h"
#include "sema.h"

unsigned int MAX_CONNECTION = 3;
List *sockets;

struct rk_sema sem;
pthread_mutex_t mutexList = PTHREAD_MUTEX_INITIALIZER;

// We want to create a send thread and a recption thread for each user
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

  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket created\n");

  struct sockaddr_in ad;
  ad.sin_family = AF_INET;
  ad.sin_addr.s_addr = INADDR_ANY;
  ad.sin_port = htons(atoi(argv[1]));
  bind(dS, (struct sockaddr *)&ad, sizeof(ad));
  printf("Socket named\n");

  listen(dS, MAX_CONNECTION);
  printf("Listening mode on port %d \n", atoi(argv[1]));

  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);
  signal(SIGINT, serverQuit);
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

    // Maybe free here
    
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
  shutdown(dS, 2);
  rk_sema_destroy(&sem);
  printf("End program\n");
}

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

void receiveMessage(void *sock_client)
{
  tsr *sock_cli = (tsr *)sock_client;
  char *msg;
  u_long size;
  char* pseudo;
  int check;
  do{
    if (recv((*sock_cli).client, &size, sizeof(u_long), 0) == -1)
    {
      redErrorMessage("Error message size received\n");
    }
    blueMessage("Size received\n");
    pseudo = (char*)malloc(sizeof(char)*size);
    if (recv((*sock_cli).client, pseudo, size, 0) == -1)
    {
      redErrorMessage("Error message received\n");
    }
    blueMessage("Pseudo received : ");
    blueMessage(pseudo);
    printf("\n");
    check = pseudoInList(sockets, pseudo);
    if (check == 0){
      sendSpecificMessage((*sock_cli).client, "Username already used !\nPlease choose another username : ");
    }
  }while(check == 0);

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
      printf("Command detected\n");
      char* copyMessage = (char*)malloc(strlen(msg)+1);
      strcpy(copyMessage,msg);
      char *strto = strtok(copyMessage, " ");
      if (strcmp(msg, "/quit") == 0)
      {
        userQuit((*sock_cli).client);
        break;
      }
      else if (strcmp(strto, "/mp") == 0)
      {
        printf("Go to private message\n");
        sendPrivateMessage(msg,(*sock_cli).client);
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
          // Execution of threads
          pthread_create(&send, NULL, transmitMessage, (void *)sendData);
        }
        current = current->next;
      }
    }
  }
  free(msg);
}

// Allows the server to stop
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

// Allows sending a private message
void sendPrivateMessage(char *msg, int client)
{
  char** mess = str_split(msg);
  char* cmd = (char*)malloc(strlen(mess[0]));
  cmd = mess[0];
  char* target = mess[1];
  int id = getIdByPseudo(sockets, target);
  if (id != NULL){
    char* pseudo = getPseudoById(sockets, client);
    int commandSize = sizeof(cmd);
    int idSize = sizeof(target);
    tss* sendData = (tss*)malloc(sizeof(tss));
    (*sendData).client = id;
    (*sendData).size = strlen(mess[2]) + 1;
    (*sendData).message = mess[2];
    (*sendData).pseudoSender = pseudo;
    pthread_t send;
    pthread_create(&send, NULL, transmitMessage, (void *)sendData);
  }
  else {
    sendSpecificMessage(client, "This user doesn't exist !");
    printf("The user doesn't exist ! \n");
  }
}

void sendSpecificMessage(int client, char* message){
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