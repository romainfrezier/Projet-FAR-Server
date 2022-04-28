#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "user.h"

#define MAX 100

int dS;

// commands :
//  - quit
//  - private message
//  - see the commands manual
//  - text in italics, bold ... (see later)

void displayManuel(){
    FILE *manual;
    char c;
    manual = fopen("command.txt","rt");
    while((c=fgetc(manual))!=EOF){
        printf("%c",c);
    }
    fclose(manual);
    printf("\n");
}

void quit(int n){
    char* m = "/quit";
    u_long size = strlen(m)+1;
    printf("Message size : %lu\n", size);
    
    // Send message size
    if(send(dS, &size, sizeof(u_long), 0) == -1)
    {
      perror("Error sending size\n");
      exit(0);
    }
    
    // Send message
    if(send(dS, m, size, 0) == -1)
    {
      perror("Error sending message\n");
      exit(0);
    }
    exit(0);
}

// Functions to print in color
// Green for sending message
void green () {
  printf("\033[0;32m");
}
// Blue for receiving message
void blue () {
  printf("\033[0;34m");
}
// Purple for receiving private message
void purple () {
  printf("\033[0;35m");
}
// Red for errors
void red () {
  printf("\033[0;31m");
}
// Reset color to white
void reset () {
  printf("\033[0m");
}

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
  dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket created\n");
  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr));
  aS.sin_port = htons(atoi(argv[2]));
  socklen_t lgA = sizeof(struct sockaddr_in);
  connect(dS, (struct sockaddr *) &aS, lgA);

  int check;
  do{
    // Connection message size reception
    u_long sizeCoMsg;
    int receive = recv(dS, &sizeCoMsg, sizeof(u_long), 0);
    if (receive == -1)
    {
      red();
      perror("Error message size received\n");
      reset();
      exit(0);
    } 
    else if (receive == 0){
      red();
      printf("Server shutdown now !\n");
      reset();
      exit(0);
    }
    // Message reception
    char* isConnected = (char*)malloc(sizeCoMsg*sizeof(char));
    if(recv(dS, isConnected, sizeCoMsg*sizeof(char), 0) == -1)
    {
      red();
      perror("An error appeared during connection to the server...\n");
      reset();
      exit(0);
    }  
    printf("%s\n", isConnected);

    check = strcmp(isConnected, "Connected !");

    if(check != 0){
      char* username = (char*)malloc(sizeof(char)*50);
      fgets(username, 50, stdin);
      username[strcspn(username, "\n")] = 0;
      printf("My username : %s \n", username);
      u_long size = strlen(username)+1;

      // Send username size
      if(send(dS, &size, sizeof(u_long), 0) == -1)
      {
        red();
        perror("Error sending username size\n");
        reset();
        exit(0);
      }
      
      // Send username
      if(send(dS, username, size, 0) == -1)
      {
        red();
        perror("Error sending username\n");
        reset();
        exit(0);
      }
    }
  }while(check != 0);

  signal(SIGINT, quit);
  
  // Execution of threads
  pthread_t send;
  pthread_t receive;
  pthread_create(&send, NULL, sendMessage, dS);
  pthread_create(&receive, NULL, receiveMessage, dS);
  
  // Waiting for the end of threads
  pthread_join(send, 0);
  pthread_join(receive, 0);

  // Shutdown of socket
  shutdown(dS,2);
  printf("End program\n");
}

void receiveMessage(int socket){
  char *m = (char*)malloc(MAX*sizeof(char));
  while (1)
  {
    // Size reception
    u_long size;
    int receive = recv(socket, &size, sizeof(u_long), 0);
    if (receive == -1)
    {
      red();
      perror("Error message size received\n");
      reset();
      exit(0);
    } 
    else if (receive == 0){
      red();
      printf("Server shutdown now !\n");
      reset();
      exit(0);
    }

    // Message reception
    char* res = (char*)malloc(size*sizeof(char));
    if(recv(socket, res, size*sizeof(char), 0) == -1)
    {
      red();
      perror("Error message received\n");
      reset();
      exit(0);
    } 
    blue();
    printf("Message received : %s\n", res);
    reset();
  }
  free(m);
}

void sendMessage(int socket){
  char *m = (char*)malloc(MAX*sizeof(char));
  while(strcmp(m,"/quit") != 0)
  {
    // User input
    printf("Enter your message (100 max) : \n");
    fgets(m, 100, stdin);
    m[strcspn(m, "\n")] = 0;
    green();
    printf("My message : %s \n", m);
    reset();
    u_long size = strlen(m)+1;

    if (strcmp(m,"/man") == 0){
      displayManuel();
    }
    else {
      // Send message size
      if(send(socket, &size, sizeof(u_long), 0) == -1)
      {
        red();
        perror("Error sending size\n");
        reset();
        exit(0);
      }
      
      // Send message
      if(send(socket, m, size, 0) == -1)
      {
        red();
        perror("Error sending message\n");
        reset();
        exit(0);
      }
    }

  }
  shutdown(socket, 2);
  free(m);
  exit(0);
}


