#ifndef FILESERVER_H_ /* Include guard */
#define FILESERVER_H_

typedef struct fileStruct fileStruct;

typedef struct sendFileStruct sendFileStruct;

struct sendFileStruct
{
    int client;
    char *filename;
    char *path;
    long fileSize;
};

struct fileStruct
{
    size_t filenameSize;
    long fileSize;
};

typedef struct thread_receiveFile trf;

struct thread_receiveFile
{
    char *fileName;
    int client;
    long fileSize;
};

void * fileGetThreadFunc(void *arg);                                  // connect socket for get file
void * fileSendThreadFunc(void *arg);                                 // connect socket for send file
void receiveFile(fileStruct *fileInfo, int client, char *filename); // prepare the receiving of the file
void *fileTransferReception(void *receiveFileData);                  // receive the file from the user
char *listFile(char *folder);                                       // list the file of the server
void * prepareSendingFile(void* data);                                // prepare the sending of the file
void sendFile(int client, fileStruct *file, char *filename);        // transfer the file to the user
char* chooseNameFile(char* nameFile, int i);                             // Choose a file name for the file transfered

#endif // FILESERVER_H_