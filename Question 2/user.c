#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <regex.h>

#include "user.h"
#include "colors.h"
#include "stringFunc.c"

#define MAX 100

int dS;
regex_t regex;

// commands :
//  - quit
//  - private message
//  - see the commands manual
//  - see all users connected (see later)
//  - text in italics, bold ... (see later)

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

  signal(SIGINT, quitForUser);

  int check;
  do{
    // Connection message size reception
    u_long sizeCoMsg;
    int receive = recv(dS, &sizeCoMsg, sizeof(u_long), 0);
    if (receive == -1)
    {
      redErrorMessage("Error message size received\n");
    } 
    else if (receive == 0){
      redErrorMessage("Server shutdown now !\n");
    }
    // Message reception
    char* isConnected = (char*)malloc(sizeCoMsg*sizeof(char));
    if(recv(dS, isConnected, sizeCoMsg*sizeof(char), 0) == -1)
    {
      redErrorMessage("An error appeared during connection to the server...\n");
    }  
    printf("%s\n", isConnected);

    check = strcmp(isConnected, "Connected !");

    if(check != 0){
      char* username = (char*)malloc(sizeof(char)*50);
      fgets(username, 50, stdin);
      username[strcspn(username, "\n")] = 0;
      printf("My username : %s \n", username);
      sendSpecificMessage(dS, username);
    }
  }while(check != 0);
  
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
    u_long sizeMessage;
    int receive = recv(socket, &sizeMessage, sizeof(u_long), 0);
    if (receive == -1)
    {
      redErrorMessage("Error message size received\n");
    } 
    else if (receive == 0){
      redErrorMessage("Server shutdown now !\n");
    }

    // Message reception
    char* messageReceive = (char*)malloc(sizeMessage*sizeof(char));
    if(recv(socket, messageReceive, sizeMessage*sizeof(char), 0) == -1)
    {
      redErrorMessage("Error message received\n");
    }

    if (strcmp(messageReceive, "/quit") == 0){
      printf("/quit server received \n");
      quitForUser(0);
    }

    int resRegexMp;

    resRegexMp = regcomp(&regex, "^(mp)[:print:]*", 0);
    resRegexMp = regexec(&regex, messageReceive, 0, NULL, 0);
    if (resRegexMp == 0)
    {
      purpleMessage(messageReceive);
    }
    else {
      blueMessage(messageReceive);
    }
    printf("\n");
    printf("Enter your message (100 max) : \n");
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
    greenMessage("My message : ");
    greenMessage(m);
    printf("\n");
    u_long size = strlen(m)+1;

    int resRegexSFile;

    resRegexSFile = regcomp(&regex, "^\/sfile[:print:]*", 0);
    resRegexSFile = regexec(&regex, m, 0, NULL, 0);
    if (strcmp(m,"/man") == 0){
      displayManuel();
    }
    else if (resRegexSFile == 0)
    {
      sendSpecificMessage(socket, "/sfile");
      int port = receivePort(socket);
      char** cmd = str_split(m,1);
      sendFileStruct data;
      data.socketServer = socket;
      data.port = port;
      data.fileName = cmd[1];
      pthread_t sendFileThread;
      pthread_create(&sendFileThread, NULL, sendFile, &data);
    }
    else {
      sendSpecificMessage(socket, m);
    }
  }
  shutdown(socket, 2);
  free(m);
  exit(0);
}

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

void displayManuel()
{
  FILE *manual;
  char c;
  manual = fopen("command.txt", "rt");
  while ((c = fgetc(manual)) != EOF)
  {
    printf("%c", c);
  }
  fclose(manual);
  printf("\n");
}

void quitForUser(int n)
{
  char *m = "/quit";
  u_long size = strlen(m) + 1;

  // Send message size
  if (send(dS, &size, sizeof(u_long), 0) == -1)
  {
    perror("Error sending size\n");
    exit(0);
  }

  // Send message
  if (send(dS, m, size, 0) == -1)
  {
    perror("Error sending message\n");
    exit(0);
  }
  exit(0);
}

void sendFile(void* sendFileData)
{
  //connexion socket
  //envoi contenu file par tcp
}

int receivePort(int socket){
  int port;
  int receive = recv(socket, &port, sizeof(int), 0);
  if (receive == -1)
  {
    redErrorMessage("Error message size received\n");
  }
  else if (receive == 0)
  {
    redErrorMessage("Server shutdown now !\n");
  }
  return port;
}