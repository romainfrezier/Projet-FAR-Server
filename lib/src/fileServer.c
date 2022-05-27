/**
 * @file fileServer.h
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief File functions implementation
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>

#include "../headers/fileServer.h"
#include "../headers/colors.h"
#include "../headers/commandServer.h"
#include "../headers/tools.h"
#include "../headers/stringFunc.h"

/**
 * @brief Size of the block of bytes to send
 */
#define SIZE 1024


void *fileGetThreadFunc(void *arg)
{
  int socket = *((int *)arg);

  while (1)
  {
    struct sockaddr_in aCfile;
    socklen_t lgFile = sizeof(struct sockaddr_in);
    int acceptation = accept(socket, (struct sockaddr *)&aCfile, &lgFile);

    // Size reception
    int size;
    if (recv(acceptation, &size, sizeof(int), 0) == -1)
    {
      redErrorMessage("Error struct size received\n");
    }
    blueMessage("Size received\n");

    // Struct reception
    fileStruct *fileInfo = (fileStruct *)malloc(size);
    if (recv(acceptation, fileInfo, size, 0) == -1)
    {
      redErrorMessage("Error struct received\n");
    }
    blueMessage("Struct received\n");

    // filename reception
    char *filename = (char *)malloc(fileInfo->filenameSize);
    if (recv(acceptation, filename, fileInfo->filenameSize, 0) == -1)
    {
      redErrorMessage("Error filename received\n");
    }
    blueMessage("Filename received\n");

    receiveFile(fileInfo, acceptation, filename);
  }

  free(arg);
  // File thread shutdown
  shutdown(socket, 2);
  return NULL;
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

void *fileTransferReception(void *receiveFileData)
{
  trf *data = (trf *)receiveFileData;
  // char *filename = chooseNameFile(data->fileName);
  char *filename = chooseNameFile(data->fileName, 1);
  int socket = data->client;

  FILE *fprecv;
  char buffer[SIZE];
  int recvBuffer;

  char *folder = "./serverStorage/";
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

    recvBuffer = recv(socket, buffer, count, 0); // receive the block of bytes from the user
    if (recvBuffer <= 0)
    {
      perror("Error in receiving buffer.");
      exit(1);
    }
    fwrite(buffer, sizeof(buffer), 1, fprecv); // write file
    bzero(buffer, count);
  }
  greenMessage("File written as ");
  greenMessage(path);
  printf("\n");
  fclose(fprecv);
//  free(path);
  return NULL;
}

char *listFile(char *folder)
{
  DIR *d;
  struct dirent *dir;
  char *fileList = "\nList of server files : \n\n";
  char *finalString = (char *)malloc(strlen(fileList));
  strcpy(finalString, fileList);
  d = opendir(folder);
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if ((strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..") != 0) && (strcmp(dir->d_name, ".DS_Store") != 0))
      {
        char *filename = (char *)malloc((strlen(dir->d_name)) * sizeof(char) + strlen("\n") + strlen("\t• "));
        strcat(filename, "\t• ");
        strcat(filename, dir->d_name);
        strcat(filename, "\n");
        finalString = (char *)realloc(finalString, (strlen(finalString) + strlen(filename)) * sizeof(char));
        strcat(finalString, filename);
//        free(filename);
      }
    }
    closedir(d);
  }
  return finalString;
}

char *chooseNameFile(char *nameFile,int i)
{
    DIR *d;
    struct dirent *dir;
    int found = 1;
    d = opendir("./serverStorage/");
    while ((dir = readdir(d)) != NULL && found != 0)
    {
        if (strcmp(nameFile, dir->d_name) == 0)
        {
            found = 0;
        }
    }
    char *arr[3];
    char *haveNumber[3];
    if (found == 0)
    {
        // change name
        getRegexGroup(arr, 3, nameFile, "^(.*)(\\..*)$");
        int regexRes = regex(arr[1], "^(.*)(-[0-9]+).*$");
        char *cpy = (char*)malloc(strlen(arr[1]));
        strcpy(cpy, arr[1]);
        if (regexRes == 0){
            getRegexGroup(haveNumber, 3, arr[1], "^(.*)(-[0-9]+)");
            strremove(cpy,haveNumber[2]);
        }
        char *number = (char*) malloc(10);
        char *format = (char*) malloc(10);
        strcat(format, "-");
        sprintf(number, "%d", i);
        strcat(format, number);
        char *newFilename = (char*)malloc(strlen(cpy)+strlen(format)+strlen(arr[2]));
        strcpy(newFilename,cpy);
        strcat(newFilename, format);
        strcat(newFilename, arr[2]);
//        free(cpy);
//        free(number);
//        free(format);
        return chooseNameFile(newFilename, i+1);
    }
    else
    {
        return nameFile;
    }
}

void *fileSendThreadFunc(void *arg)
{
  FILE *fp;
  int socket = *((int *)arg);

  while (1)
  {
    struct sockaddr_in aCfile;
    socklen_t lgFile = sizeof(struct sockaddr_in);
    int acceptation = accept(socket, (struct sockaddr *)&aCfile, &lgFile);

    // Size reception
    u_long size;
    if (recv(acceptation, &size, sizeof(u_long), 0) == -1)
    {
      redErrorMessage("Error filename size received\n");
    }
    blueMessage("Filename size received\n");

    // filename reception
    char *filename = (char *)malloc(size * sizeof(char));
    if (recv(acceptation, filename, size, 0) == -1)
    {
      redErrorMessage("Error filename received\n");
    }
    blueMessage("Filename received\n");

    // get file path
    char *folder = "serverStorage/";
    char *path = (char *)malloc((strlen(folder) + size) * sizeof(char));
    strcat(path, folder);
    strcat(path, filename);

    fp = fopen(path, "rb");
    if (fp == NULL)
    {
      redMessage("The file doesn't exist !\n");
      sendSpecificMessage(acceptation, "The file doesn't exist !");
    }
    else
    {
      sendSpecificMessage(acceptation, "The file exist !");

      fseek(fp, 0, SEEK_END);    // Jump to the end of the file
      long fileSize = ftell(fp); // Get the current byte offset in the file
      rewind(fp);                // Jump back to the beginning of the file
      fclose(fp);

      if (fileSize > 100000000)
      {
        sendSpecificMessage(acceptation, "You cannot send a file over 100MB");
      }
      else
      {
        sendSpecificMessage(acceptation, "File transfer is accepted");
        sendFileStruct *data = (sendFileStruct *)malloc(sizeof(sendFileStruct));
        data->filename = filename;
        data->client = acceptation;
        data->path = path;
        data->fileSize = fileSize;

        pthread_t sendFileThread;
        pthread_create(&sendFileThread, NULL, prepareSendingFile, data);
      }
    }
// malloc() and free() are not thread-safe functions.
// We need to protect the calls to those functions with a mutex.
// These are a problem
//    free(path);
//    free(filename);
  }

  free(arg);

  // File thread shutdown
  shutdown(socket, 2);
    return NULL;
}

void *prepareSendingFile(void *data)
{

  sendFileStruct *dataSend = (sendFileStruct *)data;
  FILE *fp;
  size_t filenameSize = strlen(dataSend->filename);

  // get the file size
  fp = fopen(dataSend->path, "rb");
  if (fp == NULL)
  {
    redMessage("The file doesn't exist !\n");
  }
  else
  {
    // fill the struct
    fileStruct *file = (fileStruct *)malloc(sizeof(fileStruct));
    file->filenameSize = filenameSize;
    file->fileSize = dataSend->fileSize;

    int structSize = sizeof(*file);
    if (send(dataSend->client, &structSize, sizeof(int), 0) == -1) // send the size of the struct
    {
      redErrorMessage("Error in sending struct size\n");
    }

    if (send(dataSend->client, file, structSize, 0) == -1) // send the struct
    {
      redErrorMessage("Error in sending struct size\n");
    }

    /* in comment because the client already has the name unless he selects it by an ID
    if (send(dataSend->client, dataSend->filename, file->filenameSize, 0) == -1) // send the filename
    {
        redErrorMessage("Error in sending filename\n");
    }
    */

    sendFile(dataSend->client, file, dataSend->filename);
  }
  return NULL;
}

void sendFile(int client, fileStruct *file, char *name)
{

  FILE *fp;
  char buffer[SIZE];
  fileStruct *fileData = file;

  char *folder = "serverStorage/";
  char *filename = name;
  long fileSize = fileData->fileSize;
  char *path = (char *)malloc((strlen(folder) + fileData->filenameSize) * sizeof(char));
  strcat(path, folder);
  strcat(path, filename);
  fp = fopen(path, "rb"); // Open the file in binary mode

  int count;
  for (int i = 0; i < fileSize; i += SIZE) // send file block by block until there are no more byts to send
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
    if (send(client, buffer, sizeof(buffer), 0) == -1) // send the block of bytes to the server
    {
      perror("Error in sending file.");
      exit(1);
    }
    bzero(buffer, count); // Reset the buffer
  }
  fclose(fp); // Close the file
//  free(path);
  greenMessage("File send succesfully\n");
}