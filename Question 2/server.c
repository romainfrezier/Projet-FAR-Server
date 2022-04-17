#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "server.h"
#include <pthread.h>

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
  printf("Listening mode\n");

  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);

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
    printf("Message received : %s", msg);

    if (strcmp(msg, "/quit\n") == 0)
    {
      delVal(sockets, (*sock_cli).client);
      shutdown((*sock_cli).client, 2);
      connection -= 1;
      break;
    }
    else
    {
      // Send to each user
      pthread_t send;
      size_t array_size = (sizeof((*sock_cli).clients)) / (sizeof((*sock_cli).clients[0]));
      Link* current = sockets->head;
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

List *createList()
{
    List *list = (List *)malloc(sizeof(List));
    list->head = NULL;
    return list;
}

int listIsEmpty(List *list)
{
    if (list->head == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void addFirst(List *list, int value)
{
    Link *link = (Link *)malloc(sizeof(Link));
    link->value = value;
    if (list->head == NULL)
    {
        list->head = link;
        link->next = NULL;
    }
    else
    {
        link->next = list->head;
        list->head = link;
    }
}

Link *next(Link *link)
{
    if (link->next == NULL)
    {
        return NULL;
    }
    else
    {
        return link->next;
    }
}

void delFirst(List *list)
{
    if (list->head != NULL)
    {
        Link *deleted = list->head;
        list->head = list->head->next;
        free(deleted);
    }
}

void delVal(List *list, int val)
{
    if (listIsEmpty(list) == 1)
    {
        if (list->head->value == val)
        {
            delFirst(list);
        }
        else
        {
            delValAux(list->head, val);
        }
    }
    else
    {
        printf("list of sockets is empty !");
    }
}

void delValAux(Link *link, int val)
{
    if (link->next != NULL)
    {
        if (link->next->value == val)
        {
            Link *deleted = link->next;
            link->next = link->next->next;
            free(deleted);
        }
        else
        {
            delValAux(link->next, val);
        }
    }
    else
    {
        printf("Value not found !");
    }
}