#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>

#include "stringFunc.h"
#include "list.h"
#include "server.h"
#include "colors.h"
#include "sema.h"

#define SIZE 1024

unsigned int MAX_CONNECTION = 3;

List *sockets;

struct rk_sema sem;
pthread_mutex_t mutexList = PTHREAD_MUTEX_INITIALIZER;
char *adminKey = "1234";
int dSfile;

// We want to create a send thread and a recption thread for each user
int main(int argc, char *argv[])
{
  // adminKey = (char *)malloc(9 * sizeof(char));
  // generateAdminKey(adminKey);

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

  dSfile = socket(PF_INET, SOCK_STREAM, 0);
  if (dSfile < 0)
  {
    redErrorMessage("ERROR opening socket file");
  }
  if (setsockopt(dSfile, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    redErrorMessage("setsockopt(SO_REUSEADDR) failed for file");
  }
  printf("Socket file created\n");

  struct sockaddr_in adFile;
  adFile.sin_family = AF_INET;
  adFile.sin_addr.s_addr = INADDR_ANY;
  adFile.sin_port = htons(atoi(argv[1]) + 1);
  bind(dSfile, (struct sockaddr *)&adFile, sizeof(adFile));
  printf("File socket named\n");

  listen(dS, MAX_CONNECTION);
  listen(dSfile, MAX_CONNECTION);

  printf("Listening mode on port %d \n", atoi(argv[1]));
  greenMessage("The admin key is : ");
  greenMessage(adminKey);
  printf("\n");
  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);
  signal(SIGINT, serverQuit);
  pthread_t fileThread;
  pthread_create(&fileThread, NULL, fileThreadFunc, NULL);
  while (1) // TODO : Maybe another condition
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
  free(adminKey);
  shutdown(dS, 2);
  rk_sema_destroy(&sem);
  printf("End program\n");
}

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
      printf("Command detected\n");
      char *copyMessage = (char *)malloc(strlen(msg) + 1);
      strcpy(copyMessage, msg);
      char *strto = strtok(copyMessage, " ");
      if (strcmp(msg, "/quit") == 0)
      {
        userQuit((*sock_cli).client);
        break;
      }
      else if (strcmp(strto, "/mp") == 0)
      {
        printf("Go to private message\n");
        sendPrivateMessage(msg, (*sock_cli).client);
      }
      else if (strcmp(strto, "/admin") == 0)
      {
        printf("Go to admin verification\n");
        adminVerification(msg, (*sock_cli).client);
      }
      else if (strcmp(strto, "/kick") == 0)
      {
        printf("Go to kick function\n");
        kick(msg, (*sock_cli).client);
      }
      else if (strcmp(strto, "/users") == 0)
      {
        printf("Go to displayUsers function \n");
        displayAllUsers((*sock_cli).client);
      }
      else if (strcmp(strto, "/files") == 0)
      {
        printf("Go to list file function \n");
        sendSpecificMessage((*sock_cli).client, listFile("./serverStorage"));
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
  exit(0);
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
      char *mpPseudo = (char *)malloc(sizeof(char) * (strlen(pseudo) + strlen("(mp) ")));
      strcat(mpPseudo, "(mp) ");
      strcat(mpPseudo, pseudo);
      int commandSize = sizeof(cmd);
      int idSize = sizeof(target);
      tss *sendData = (tss *)malloc(sizeof(tss));
      (*sendData).client = id;
      (*sendData).size = strlen(mess[2]) + 1;
      (*sendData).message = mess[2];
      (*sendData).pseudoSender = mpPseudo;
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
    sendSpecificMessage(client, "The command is : [/mp targetPseudo yourMessage] \n");
  }
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

void adminVerification(char *message, int client)
{
  if (verifCommand(message, 1) == 1)
  {
    char **mess = str_split(message, 2);
    if (strcmp(adminKey, mess[1]) == 0)
    {
      char *pseudo = getPseudoById(sockets, client);
      if (pseudo != NULL)
      {
        setUserAdmin(sockets, client);
        sendSpecificMessage(client, "You are now an admin !\n");
      }
    }
    else
    {
      sendSpecificMessage(client, "That's not the current admin key !\n");
    }
  }
  else
  {
    sendSpecificMessage(client, "The command is : [/admin adminKey] \n");
  }
}

void kick(char *message, int client)
{
  if (verifCommand(message, 1) == 1)
  {
    if (isClientAdmin(sockets, client) == 1)
    {
      char **mess = str_split(message, 2);
      int idKickedClient = getIdByPseudo(sockets, mess[1]);
      if (idKickedClient != NULL)
      {
        sendSpecificMessage(idKickedClient, "You have been kicked by an admin !\n");
        printf("User %d has been kicked by admin %d \n", idKickedClient, client);
        sendSpecificMessage(idKickedClient, "/quit");
        sendSpecificMessage(client, "The user has been kicked !\n");
      }
      else
      {
        sendSpecificMessage(client, "The user doesn't exist !\n");
      }
    }
    else
    {
      sendSpecificMessage(client, "You don't have the permission !\n");
    }
  }
  else
  {
    sendSpecificMessage(client, "The command is : [/kick targetPseudo] \n");
  }
}

void displayAllUsers(int client)
{
  sendSpecificMessage(client, getAllUsers(sockets));
}

char* listFile(char *folder){
  DIR *d;
  struct dirent *dir;
  char *fileList = "List of server files : \n\n";
  char *finalString = (char *)malloc(strlen(fileList));
  strcpy(finalString, fileList);
  d = opendir("./serverStorage");
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if ((strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..") != 0)){
        char *filename = (char *)malloc((strlen(dir->d_name)) * sizeof(char) + strlen("\n") + strlen("  - "));
        strcat(filename, "  - ");
        strcat(filename, dir->d_name);
        strcat(filename, "\n");
        finalString = (char *)realloc(finalString, (strlen(finalString) + strlen(filename)) * sizeof(char));
        strcat(finalString, filename);
      }
    }
    closedir(d);
  }
  return finalString;
}

void generateAdminKey(char *key)
{
  for (int i = 0; i < 10; i++)
  {
    char randomletter = 'A' + (rand() % 26);
    strcat(key, &randomletter);
  }
}

void fileThreadFunc()
{
  while (1)
  {
    struct sockaddr_in aCfile;
    socklen_t lgFile = sizeof(struct sockaddr_in);
    int acceptation = accept(dSfile, (struct sockaddr *)&aCfile, &lgFile);

    // ------------- Recevoir la taille d'une struct, une struct, puis filename -------------
    // Size reception
    int size;
    if (recv(acceptation, &size, sizeof(int), 0) == -1)
    {
      redErrorMessage("Error struct size received\n");
    }
    blueMessage("Size received\n");

    // Struct reception
    fileStruct *fileInfo = (fileStruct*)malloc(size);
    if (recv(acceptation, fileInfo, size, 0) == -1)
    {
      redErrorMessage("Error struct received\n");
    }
    blueMessage("Struct received\n");

    // filename reception
    char *filename = (char*)malloc(fileInfo->filenameSize);
    if (recv(acceptation, filename, fileInfo->filenameSize, 0) == -1)
    {
      redErrorMessage("Error filename received\n");
    }
    blueMessage("Filename received\n");
    // --------------------------------------------------------------------------------------

    receiveFile(fileInfo, acceptation, filename);
    }

  // File thread shutdown
  shutdown(dSfile, 2);
}

void receiveFile(fileStruct* fileInfo, int client, char* filename)
{
  fileStruct* file = fileInfo;
  pthread_t receiveThread;
  trf receiveFileData;
  receiveFileData.client = client;
  receiveFileData.fileSize = file->fileSize;
  receiveFileData.fileName = filename;
  pthread_create(&receiveThread, NULL, fileTransferReception, &receiveFileData);
  pthread_join(receiveThread, 0);
  shutdown(client,2);
}

void fileTransferReception(void *receiveFileData)
{
  trf *data = (trf *)receiveFileData;
  char *filename = data->fileName;
  int socket = data->client;

  FILE *fprecv;
  char buffer[SIZE];
  int recvBuffer;

  char *folder = "serverStorage/";
  char *path = (char *)malloc((strlen(folder) + strlen(filename)) * sizeof(char));
  strcat(path, folder);
  strcat(path, filename);

  long fileSize = data->fileSize;
  fprecv = fopen(path, "w+"); // open the file in "path" to write inside. Overwrite it if it already exists, create it if not

  int count;
  for (int i = 0; i < fileSize; i += SIZE) // receive file block by block of SIZE byts until there are no more byts to receive
  {
    if (i + SIZE < fileSize) // Calculate the size of the block to receive
    {
      count = SIZE;
    }
    else
    {
      count = fileSize - i;
    }

    recvBuffer = recv(socket, buffer, SIZE, 0); // receive the block of bytes from the user
    if (recvBuffer <= 0)
    {
      perror("Error in receiving buffer.");
      exit(1);
    }
    fwrite(buffer, sizeof(buffer), 1, fprecv); // write file
    bzero(buffer, SIZE);
  }
  greenMessage("File written as ");
  greenMessage(path);
  printf("\n");
  fclose(fprecv);
  return;
}
