#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "client.h"

#define MAX 100

// We want a thread that manages the shipment and another who manages the receipt
int main(int argc, char *argv[]) {

  if (argc!=3) 
  {
    perror("Usage : ./exe IP port");
  }

  if (atoi(argv[2])<=1024) 
  {
    perror("Bad port: must be greater than 1024");
  }

  printf("Start program\n");
  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket created\n");

  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
  aS.sin_port = htons(atoi(argv[2])) ;
  socklen_t lgA = sizeof(struct sockaddr_in) ;
  connect(dS, (struct sockaddr *) &aS, lgA) ;
  printf("Socket connected\n");
  
  // Execution of threads
  pthread_t send;
  pthread_t receive;
  pthread_create(&send, NULL, sendMessage, dS);
  pthread_create(&receive, NULL, receiveMessage, dS);
  
  // Waiting for the end of threads
  pthread_join(send, 0);
  pthread_join(receive, 0);

  // Shutdown of socket
  shutdown(dS,2) ;
  printf("End program\n");
}

void receiveMessage(int socket){
  char *m = (char*)malloc(MAX*sizeof(char));
  while (1)
  {
    // Size reception
    u_long size;
    if(recv(socket, &size, sizeof(u_long), 0) == -1)
    {
      perror("Error message size received\n");
      exit(0);
    } 

    // Message reception
    char* res = (char*)malloc(size*sizeof(char));
    if(recv(socket, res, size*sizeof(char), 0) == -1)
    {
      perror("Error message received\n");
      exit(0);
    } 
    printf("Message received : %s\n", res);
  }
  free(m);
}

void sendMessage(int socket){
  char *m = (char*)malloc(MAX*sizeof(char));
  while(1)
  {
    // User input
    printf("Enter your message (100 max) : \n");
    fgets(m, 100, stdin);
    printf("My message : %s \n", m);
    u_long taille = strlen(m)+1;
    printf("Message size : %lu\n", taille);
    
    // Send message size
    if(send(socket, &taille, sizeof(u_long), 0) == -1)
    {
      perror("Error sending size\n");
      exit(0);
    }
    
    // Send message
    if(send(socket, m, taille, 0) == -1)
    {
      perror("Error sending message\n");
      exit(0);
    }

    if (strcmp(m,"/quit") == 0) {
      printf("leave the messagery");
      exit(0);
    }
    else {
      printf("Message send \n");
    }
  }
  free(m);
}


