#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "fileServer.h"
#include "server.h"

#define SIZE 1024

void fileTransferReception(int socket, char *filename)
{
    FILE *fprecv;
    char buffer[SIZE];
    int recvBuffer;

    char *folder = "serverStorage/";
    char *path = (char *)malloc((strlen(folder) + strlen(filename)) * sizeof(char));
    strcat(path, folder);
    strcat(path, filename);

    u_long fileSize;                                               // size of the file
    int recvFileSize = recv(socket, &fileSize, sizeof(u_long), 0); // receive the file size
    if (recvFileSize <= 0)
    {
        perror("Error in receiving file size.");
        exit(1);
    }

    fprecv = fopen(filename, "w+"); // open the file "filename" to write inside. Overwrite it if it already exists, create it if not

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
    fclose(fprecv);
    return;
}

void receiveFile(char *msg, int client)
{
    sendSpecificMessage(client, msg);
    pthread_t receiveThread;
    char **cmd = str_split(msg, 1);
    trf receiveFileData;
    receiveFileData.fileName = cmd[1];
    receiveFileData.client = client;
    pthread_create(&receiveThread, NULL, receiveFileThread, &receiveFileData);
}

void receiveFileThread(void *receiveFileData, int dSfile)
{
    socklen_t lgFile = sizeof(struct sockaddr_in);
    trf *data = (trf *)receiveFileData;
    struct sockaddr_in aCfile;
    sendSpecificMessage(data->client, "Ready");
    int acceptationFile = accept(dSfile, (struct sockaddr *)&aCfile, &lgFile);
    fileTransferReception(acceptationFile, data->fileName);
}
