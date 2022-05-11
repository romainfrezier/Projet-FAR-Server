#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <regex.h>
#include <ifaddrs.h>

#include "user.h"
#include "colors.h"
#include "stringFunc.c"

#define MAX 100
#define SIZE 1024

int dS;
int dSFileSend;

regex_t regex;
pthread_t fileThread;

struct sockaddr_in aS;

char *ipAddress;
int portSendingFile;

// We want a thread that manages the shipment and another who manages the receipt
int main(int argc, char *argv[])
{

  if (argc != 3)
  {
    perror("Usage : ./exe IP port");
  }

  if (atoi(argv[2]) <= 1024)
  {
    perror("Bad port: must be greater than 1024");
  }

  printf("Start program\n");

  int enable = 1;

  dS = socket(PF_INET, SOCK_STREAM, 0);
  if (dS < 0)
  {
    redErrorMessage("ERROR opening socket for sending file");
  }
  if (setsockopt(dS, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed");
  }
  printf("Socket for message created\n");

  // socket for sending file
  ipAddress = argv[1];
  portSendingFile = atoi(argv[2]) + 1;
  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET, ipAddress, &(aS.sin_addr));
  aS.sin_port = htons(atoi(argv[2]));
  socklen_t lgA = sizeof(struct sockaddr_in);
  connect(dS, (struct sockaddr *)&aS, lgA);

  dSFileSend = socket(PF_INET, SOCK_STREAM, 0);
  if (dSFileSend < 0)
  {
    redErrorMessage("ERROR opening socket for sending file");
  }
  if (setsockopt(dSFileSend, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed");
  }
  printf("Socket for file created\n");

  signal(SIGINT, quitForUser);

  int check;
  do
  {
    // Connection message size reception
    u_long sizeCoMsg;
    int receive = recv(dS, &sizeCoMsg, sizeof(u_long), 0);
    if (receive == -1)
    {
      redErrorMessage("Error message size received\n");
    }
    else if (receive == 0)
    {
      redErrorMessage("Server shutdown now !\n");
    }
    // Message reception
    char *isConnected = (char *)malloc(sizeCoMsg * sizeof(char));
    if (recv(dS, isConnected, sizeCoMsg * sizeof(char), 0) == -1)
    {
      redErrorMessage("An error appeared during connection to the server...\n");
    }
    printf("%s\n", isConnected);

    check = strcmp(isConnected, "Connected !");

    if (check != 0)
    {
      char *username = (char *)malloc(sizeof(char) * 50);
      fgets(username, 50, stdin);
      username[strcspn(username, "\n")] = 0;
      printf("My username : %s \n", username);
      sendSpecificMessage(dS, username);
    }
  } while (check != 0);

  // Execution of threads
  pthread_t send;
  pthread_t receive;
  pthread_create(&send, NULL, sendMessage, dS);
  pthread_create(&receive, NULL, receiveMessage, dS);

  // Waiting for the end of threads
  pthread_join(send, 0);
  pthread_join(receive, 0);

  // Shutdown of socket
  shutdown(dS, 2);
  printf("End program\n");
}

void sendMessage(int socket)
{
  char *m = (char *)malloc(MAX * sizeof(char));

  int resRegexSFile;
  while (strcmp(m, "/quit") != 0)
  {
    // User input
    printf("Enter your message (100 max) : \n");
    fgets(m, 100, stdin);
    m[strcspn(m, "\n")] = 0;
    greenMessage("My message : ");
    greenMessage(m);
    printf("\n");
    u_long size = strlen(m) + 1;

    resRegexSFile = regcomp(&regex, "^\/sfile[:print:]*", 0);
    resRegexSFile = regexec(&regex, m, 0, NULL, 0);

    if (strcmp(m, "/man") == 0)
    {
      displayManuel();
    }
    else if (resRegexSFile == 0)
    {
      // send socket data to the server
      printf("Go to create socket function ! \n");
      sendFileStruct data;
      data.socketServer = socket;
      data.cmd = m;
      connectSocketFile(&data);
    }
    else
    {
      sendSpecificMessage(socket, m);
    }
  }
  shutdown(socket, 2);
  free(m);
  exit(0);
}

void receiveMessage(int socket)
{
  char *m = (char *)malloc(MAX * sizeof(char));
  while (1)
  {
    // Size reception
    u_long sizeMessage;
    int receive = recv(socket, &sizeMessage, sizeof(u_long), 0);
    if (receive == -1)
    {
      redErrorMessage("Error message size received\n");
    }
    else if (receive == 0)
    {
      redErrorMessage("Server shutdown now !\n");
    }

    // Message reception
    char *messageReceive = (char *)malloc(sizeMessage * sizeof(char));
    if (recv(socket, messageReceive, sizeMessage * sizeof(char), 0) == -1)
    {
      redErrorMessage("Error message received\n");
    }

    if (strcmp(messageReceive, "/quit") == 0)
    {
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
    else
    {
      blueMessage(messageReceive);
    }
    printf("\n");
    printf("Enter your message (100 max) : \n");
  }
  free(m);
}

void connectSocketFile(sendFileStruct* data)
{
  printf("message : %s \n", data->cmd);

  // value for connection to the socket
  struct sockaddr_in aSfile;
  aSfile.sin_family = AF_INET;
  inet_pton(AF_INET, ipAddress, &(aSfile.sin_addr));
  aSfile.sin_port = htons(portSendingFile);
  socklen_t lgFile = sizeof(struct sockaddr_in);

  // Tests
  printf("ip : %s\n", ipAddress);
  printf("port htons : %d\n", portSendingFile);
  printf("arriving in connectsocket function ! \n");

  int resCoFile = connect(dSFileSend, (struct sockaddr *)&aSfile, lgFile);  // connection to the socket
  if (resCoFile == 0)
  {
    greenMessage("Begin of the file transfert\n");
    pthread_create(&fileThread, NULL, sendFile, data);
  }
  else
  {
    redMessage("Error connexion server for sending file ! \n");
  }
}

void sendFile(void *sendFileData)
{
  FILE *fp;
  sendFileStruct *data = (sendFileStruct *)sendFileData;
  char **mess = str_split(data->cmd, 1);
  
  size_t filenameSize = strlen(mess[1]);
  
  // get the file size
  char *folder = "userStorage/";
  char *path = (char *)malloc((strlen(folder) + filenameSize) * sizeof(char));
  strcat(path, folder);
  strcat(path, mess[1]);
  
  fp = fopen(path, "rb");
  fseek(fp, 0, SEEK_END);      // Jump to the end of the file
  long fileSize = ftell(fp);   // Get the current byte offset in the file
  rewind(fp);                  // Jump back to the beginning of the file
  fclose(fp);
  
  // ------------------------ Send fileStruct ------------------------

  // Tests
  printf("mess[1] : %s\n", mess[1]);
  printf("fileSize : %ld\n", fileSize);
  printf("filenameSize : %zu\n", filenameSize);

  // fill the struct
  fileStruct *file = (fileStruct*)malloc(sizeof(fileStruct));
  file->filename = mess[1];
  file->filenameSize = filenameSize;
  file->fileSize = fileSize;

  int structSize = sizeof(fileStruct);
  if (send(dSFileSend, &structSize, sizeof(int), 0) == -1) // send the size of the struct
  {
    perror("Error in sending struct size.");
    exit(1);
  }
  if (send(dSFileSend, file, structSize, 0) == -1) // send the struct
  {
    perror("Error in sending struct size.");
    exit(1);
  }

  // -----------------------------------------------------------------
  fileTransfer(dSFileSend, file);
  printf("Enter your message (100 max) :\n");
}

void fileTransfer(int socket, fileStruct *file)
{
  printf("Into fileTransfer()\n");
  FILE *fp;
  char buffer[SIZE];
  fileStruct *fileData = file;

  char *folder = "userStorage/";
  char *filename = fileData->filename;
  long fileSize = fileData->fileSize;
  char *path = (char *)malloc((strlen(folder) + fileData->filenameSize) * sizeof(char));
  strcat(path, folder);
  strcat(path, filename);
  printf("path : %s\n", path);
  fp = fopen(path, "rb");   // Open the file in binary mode
  printf("here 2\n");

  int count;
  for (int i = 0; i < fileSize; i += SIZE) // send file block by block of SIZE byts until there are no more byts to send
  {
    if (i + SIZE < fileSize) // Calculate the size of the block to send
    {
      count = SIZE;
    }
    else
    {
      count = fileSize - i;
    }

    fread(buffer, count, 1, fp);                       // read the file
    if (send(socket, buffer, sizeof(buffer), 0) == -1) // send the block of bytes to the server
    {
      perror("Error in sending file.");
      exit(1);
    }
    bzero(buffer, SIZE); // Reset the buffer
    printf("here 4\n");
  }
  fclose(fp); // Close the file
  printf("here 5\n");
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


