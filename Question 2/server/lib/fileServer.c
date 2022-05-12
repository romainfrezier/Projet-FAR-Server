#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>

#include "fileServer.h"
#include "colors.h"

#define SIZE 1024

void fileThreadFunc(void* arg)
{
  int socket = *((int *)arg);

  while (1)
  {
    struct sockaddr_in aCfile;
    socklen_t lgFile = sizeof(struct sockaddr_in);
    int acceptation = accept(socket, (struct sockaddr *)&aCfile, &lgFile);

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

  free(arg);
  // File thread shutdown
  shutdown(socket, 2);
}

void receiveFile(fileStruct *fileInfo, int client, char *filename)
{
  fileStruct *file = fileInfo;
  pthread_t receiveThread;
  trf receiveFileData;
  receiveFileData.client = client;
  receiveFileData.fileSize = file->fileSize;
  receiveFileData.fileName = filename;
  pthread_create(&receiveThread, NULL, fileTransferReception, &receiveFileData);
  pthread_join(receiveThread, 0);
  shutdown(client, 2);
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

char *listFile(char *folder)
{
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
      if ((strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..") != 0))
      {
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