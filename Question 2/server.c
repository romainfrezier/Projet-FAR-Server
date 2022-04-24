#include "stringFunc.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "list.h"
#include "server.h"
// #include "command.h"

#define MAX_CONNECTION 3

int connection = 0;
List *sockets;

// We want to create a send thread and a recption thread for each user
int main(int argc, char *argv[])
{

  // Definition of the socket array to the desired size
  sockets = createList();

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
    while (connection < MAX_CONNECTION)
    {
      int acceptation = accept(dS, (struct sockaddr *)&aC, &lg);
      addFirst(sockets, acceptation);
      printf("User %d connected with id : %d\n", connection + 1, acceptation);

      // and we attributed a reception thread to each
      tsr *receiveData = (tsr *)malloc(sizeof(tsr));
      (*receiveData).client = acceptation;
      (*receiveData).clients = sockets;
      pthread_t receive;
      pthread_create(&receive, NULL, receiveMessage, (void *)receiveData);
      connection += 1;

      // Maybe free here
    }
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
      for (int i = 0; i < connection; i++)
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
  connection -= 1;
}

// Allows sending a private message
void sendPrivateMessage(char *msg)
{
  printf("je suis dans le private\n");
  printf("définitions finies\n");
  char** mess = str_split(msg);
  char* cmd = (char*)malloc(strlen(mess[0]));
  cmd = mess[0];
  int target = atoi(mess[1]);
  printf("définitions finies\n");
  // 6 = /mp" "id" "
  // mp 123 salut ça va
  // je pense qu'il ne compte pas les espaces
  int commandSize = sizeof(cmd);
  int idSize = sizeof(target);
  printf("id : %d\n", target);
  printf("Command : %s\n", cmd);
  printf("message privé : %s\n", mess[2]);
  tss* sendData = (tss*)malloc(sizeof(tss));
  (*sendData).client = target;
  (*sendData).size = strlen(mess[2]);
  (*sendData).message = mess[2];
  pthread_t send;
  pthread_create(&send, NULL, transmitMessage, (void *)sendData);
}