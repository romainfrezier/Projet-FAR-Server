#include "stringFunc.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "list.h"
#include "server.h"
// #include "command.h"

unsigned int MAX_CONNECTION = 3;
List *sockets;
sem_t sem;

// We want to create a send thread and a recption thread for each user
int main(int argc, char *argv[])
{

  // Definition of the socket array to the desired size
  sockets = createList(MAX_CONNECTION);
  int pshared = 0; 
  sem_init(&sem, pshared, MAX_CONNECTION);
  if (argc != 2)
  {
    printf("Usage : ./exe port");
    exit(0);
  }

  if (atoi(argv[1]) <= 1024)
  {
    perror("Bad port: must be greater than 1024");
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
    sem_wait(&sem);
    int acceptation = accept(dS, (struct sockaddr *)&aC, &lg);
    
    // Send connection message
    if(send(acceptation, "Connected !\nPlease choose your username : ", 44, 0) == -1)
    {
      perror("Error sending connection message\n");
      exit(0);
    }

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
  sem_destroy(sem);
  printf("End program\n");
}

void transmitMessage(void *sock_client)
{
  tss *sock_cli = (tss *)sock_client;

  // Transmit message size
  if (send((*sock_cli).client, &((*sock_cli).size), sizeof(u_long), 0) == -1)
  {
    printf("Error sending size for socket : %d\n", (*sock_cli).client);
    exit(0);
  }

  // Transmit message size
  if (send((*sock_cli).client, (*sock_cli).message, (*sock_cli).size, 0) == -1)
  {
    perror("Error sending message\n");
    exit(0);
  }
  printf("Message transmitted\n");
}

void receiveMessage(void *sock_client)
{
  tsr *sock_cli = (tsr *)sock_client;
  char *msg;
  u_long size;
  char* pseudo;
  if (recv((*sock_cli).client, &size, sizeof(u_long), 0) == -1)
  {
    perror("Error message size received\n");
    exit(0);
  }
  printf("Size received : %lu\n", size);
  pseudo = (char*)malloc(sizeof(char)*size);
  if (recv((*sock_cli).client, pseudo, size, 0) == -1)
  {
    perror("Error message received\n");
    exit(0);
  }
  printf("Pseudo received : %s\n", pseudo);
  //add the client to the socket list
  addFirst(sockets, sock_cli->client, pseudo);
  printf("User connected with id : %d\n", sock_cli->client);

  while (1)
  {
    // Size reception
    if (recv((*sock_cli).client, &size, sizeof(u_long), 0) == -1)
    {
      perror("Error message size received\n");
      exit(0);
    }
    printf("Size received : %lu\n", size);

    // Message reception
    char *msg = (char *)malloc(size);
    if (recv((*sock_cli).client, msg, size, 0) == -1)
    {
      perror("Error message received\n");
      exit(0);
    }
    printf("Message received : %s\n", msg);

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
        printf("go to private message\n");
        sendPrivateMessage(msg);
      }
    }
    else
    {
      // Send to each user
      pthread_t send;
      size_t array_size = (sizeof((*sock_cli).clients)) / (sizeof((*sock_cli).clients[0]));
      Link *current = sockets->head;
      for (int i = 0; i < sockets->size; i++)
      {
        if (current->value != (*sock_cli).client)
        {
          tss *sendData = (tss *)malloc(sizeof(tss));
          (*sendData).client = current->value;
          (*sendData).size = size;
          (*sendData).message = msg;

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
  printf(" value du current : %d\n", current->value);
  while (current != NULL)
  {
    Link *next = current->next;
    shutdown(current->value, 2);
    printf("User %d has been stopped\n", current->value);
    delVal(sockets, current->value);
    current = next;
  }
  printf("The server has been stopped !\n");
  exit(0);
}

// Allows a user to leave the server
void userQuit(int socket)
{
  delVal(sockets, socket);
  shutdown(socket, 2);
  sem_post(&sem);
}

// Allows sending a private message
void sendPrivateMessage(char *msg)
{
  char** mess = str_split(msg);
  char* cmd = (char*)malloc(strlen(mess[0]));
  cmd = mess[0];
  int target = atoi(mess[1]);
  int commandSize = sizeof(cmd);
  int idSize = sizeof(target);
  tss* sendData = (tss*)malloc(sizeof(tss));
  (*sendData).client = target;
  (*sendData).size = strlen(mess[2]);
  (*sendData).message = mess[2];
  pthread_t send;
  pthread_create(&send, NULL, transmitMessage, (void *)sendData);
}