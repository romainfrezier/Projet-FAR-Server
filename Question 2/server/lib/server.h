#ifndef SERVER_H_ /* Include guard */
#define SERVER_H_

#include <pthread.h>

typedef struct thread_sockets_receive tsr;

struct thread_sockets_receive
{
    int client;
    List* clients;
};

typedef struct thread_sockets_send tss;

struct thread_sockets_send
{
    int size;
    char *message;
    int client;
    char* pseudoSender;
};

// typedef struct thread_receiveFile trf;

// struct thread_receiveFile {
//     char* fileName;
//     int client;
//     long fileSize;
// };

// typedef struct fileStruct fileStruct;

// struct fileStruct
// {
//     size_t filenameSize;
//     long fileSize;
// };

extern struct rk_sema sem;
extern pthread_mutex_t mutexList;
extern List *sockets;

void receiveMessage(void *sock_client);  // Reception of a user message
void transmitMessage(void* sock_client); // Sending a message to the user
// void serverQuit(int n); // Allows the server to stop
// void userQuit(int socket); // Allows a user to leave the server
void sendPrivateMessage(char *msg, int client); // Allows sending a private message
void sendSpecificMessage(int client, char *message); // Send a specific message to user selected
void executeError(char *errorMessage); // Display an error
// void displayAllUsers(int client); // display all the users connected for the user

#endif // SERVER_H_