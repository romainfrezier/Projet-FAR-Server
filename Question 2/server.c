#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include <pthread.h>

#define MAX_CONNECTION 3

int connection = 0;
int *sockets;
// We want to create a send thread and a recption thread for each user
int main(int argc, char *argv[]) {

  // Definition of the socket array to the desired size
  sockets = (int*)malloc(sizeof(int)*MAX_CONNECTION);

  if (argc!=2) 
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
  ad.sin_addr.s_addr = INADDR_ANY ;
  ad.sin_port = htons(atoi(argv[1])) ;
  bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
  printf("Socket named\n");

  listen(dS, MAX_CONNECTION) ;
  printf("Listening mode\n");

  struct sockaddr_in aC ;
  socklen_t lg = sizeof(struct sockaddr_in) ;

  while (1)
  {
    // Users are accepted until max allow
    while (connection < MAX_CONNECTION)
    {
      int acceptation = accept(dS, (struct sockaddr *)&aC, &lg);
      sockets[connection] = acceptation;
      printf("User %d connected with id : %d\n", connection + 1, sockets[connection]);

      // and we attributed a reception thread to each
      tsr *receiveData = (tsr *)malloc(sizeof(tsr));
      (*receiveData).client = sockets[connection];
      (*receiveData).clients = sockets;
      pthread_t receive;
      pthread_create(&receive, NULL, receiveMessage, (void *)receiveData);
      connection += 1;

      // Maybe free here
    }
  }
  // Shutdown of all user sockets
  for (int i = 0; i < sizeof(sockets); i++)
  {
    shutdown(sockets[i], 2);
  }

  // Server shutdown
  shutdown(dS, 2);
  printf("End program\n");
}

void transmitMessage(void* sock_client){
  tss* sock_cli = (tss*)sock_client;
  
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

void receiveMessage(void* sock_client){
  tsr* sock_cli = (tsr*)sock_client;
  char *msg;
  u_long size;
  while(1)
  {
    // Size reception
    if(recv((*sock_cli).client, &size, sizeof(u_long),0) == -1)
    {
      perror("Error message size received\n");
      exit(0);
    }
    printf("Size received : %lu\n", size);
    
    // Message reception
    char* msg = (char*)malloc(size);
    if(recv((*sock_cli).client, msg, size, 0) == -1)
    {
      perror("Error message received\n");
      exit(0);
    }
    printf("Message received : %s", msg);

    // Send to each user
    pthread_t send;
    size_t array_size = (sizeof((*sock_cli).clients)) / (sizeof((*sock_cli).clients[0])); 
    for (int i = 0; i < connection; i++)
    {
      if ((*sock_cli).clients[i] != (*sock_cli).client)
      {
        tss *sendData = (tss *)malloc(sizeof(tss));
        (*sendData).client = (*sock_cli).clients[i];
        (*sendData).size = size;
        (*sendData).message = msg;

        // Execution of threads
        pthread_create(&send, NULL, transmitMessage, (void *)sendData);
      }
    }
  }
  free(msg);
}