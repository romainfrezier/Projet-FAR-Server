/**
 * @file fileServer.h
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief File functions
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef FILESERVER_H_ /* Include guard */
#define FILESERVER_H_
 /**
  * @brief Structure for sending file to the client
  */
typedef struct sendFileStruct sendFileStruct;

/**
 * @brief Structure for sending file to the client
 *
 * @param client : id of the client who will receive file
 * @param filename : the name of the file to send
 * @param path : path of the file to send
 * @param fileSize : size of the file to send
 */
struct sendFileStruct
{
    int client;
    char *filename;
    char *path;
    long fileSize;
};

/**
 * @brief Structure that will be send to the client who receive file
 */
typedef struct fileStruct fileStruct;

/**
 * @brief Structure that will be send to the client who receive file
 *
 * @param filenameSize : size of the file name
 * @param fileSize : size of the file to send
 */
struct fileStruct
{
    size_t filenameSize;
    long fileSize;
};

typedef struct thread_receiveFile trf;

/**
 * @brief Structure for receiving file from the client
 *
 * @param fileName : the name of the file to send
 * @param client : id of the client who will receive file
 * @param fileSize : size of the file to send
 */
struct thread_receiveFile
{
    char *fileName;
    int client;
    long fileSize;
};

/**
 * @brief Connect socket for get file
 *
 * @param arg : socket for receiving file
 * @return NULL
 */
void * fileGetThreadFunc(void *arg);

/**
 * @brief Connect socket for send file
 *
 * @param arg : socket for sending file
 * @return NULL
 */
void * fileSendThreadFunc(void *arg);

/**
 * @brief Prepare the receiving of the file
 *
 * @param fileInfo : structure of file information
 * @param client : id of the client who send file
 * @param filename : file name
 */
void receiveFile(fileStruct *fileInfo, int client, char *filename);

/**
 * @brief Receive the file from the user
 *
 * @param receiveFileData : structure of the file data that the server receive
 * @return NULL
 */
void *fileTransferReception(void *receiveFileData);

/**
 * @brief List the file of a folder
 * @param folder : folder of the server storage
 * @return string of the whole file list
 */
char *listFile(char *folder);

/**
 * @brief Prepare the sending of the file
 * @param data : structure of the file data that the server send
 * @return NULL
 */
void * prepareSendingFile(void* data);

/**
 * @brief Transfer the file to the user
 *
 * @param client : id of the client who will receive file
 * @param file : structure of the file data that the client will reveive
 * @param filename : the filename selected by the client
 */
void sendFile(int client, fileStruct *file, char *filename);

/**
 * @brief Choose a file name for the file transferred, with the objective to not overwrite the file
 *
 * @param nameFile : the file name of the file that the client send
 * @param i : used for recursive call. Initial value need to be : 1
 * @return the new filename
 */
char* chooseNameFile(char* nameFile, int i);

#endif // FILESERVER_H_