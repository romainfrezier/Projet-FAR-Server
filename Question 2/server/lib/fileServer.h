#ifndef FILESERVER_H_ /* Include guard */
#define FILESERVER_H_

typedef struct fileStruct fileStruct;

struct fileStruct
{
    size_t filenameSize;
    long fileSize;
};

typedef struct thread_receiveFile trf;

struct thread_receiveFile {
    char* fileName;
    int client;
    long fileSize;
};


void fileThreadFunc(void* arg);
void receiveFile(fileStruct *fileInfo, int client, char *filename);
void fileTransferReception(void *receiveFileData);
char *listFile(char *folder);

#endif // FILESERVER_H_