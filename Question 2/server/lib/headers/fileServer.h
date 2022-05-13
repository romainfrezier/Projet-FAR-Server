#ifndef FILESERVER_H_ /* Include guard */
#define FILESERVER_H_

typedef struct fileStruct fileStruct;

typedef struct sendFileStruct sendFileStruct;

struct sendFileStruct
{
    int client;
    char *filename;
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

void fileGetThreadFunc(void *arg);
void fileSendThreadFunc(void *arg);
void receiveFile(fileStruct *fileInfo, int client, char *filename);
void fileTransferReception(void *receiveFileData);
char *listFile(char *folder);
void prepareSendingFile(void* data);
void sendFile(int client, fileStruct* file, char* filename);

#endif // FILESERVER_H_