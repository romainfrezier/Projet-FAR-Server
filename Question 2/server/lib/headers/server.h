#ifndef SERVER_H_ /* Include guard */
#define SERVER_H_

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

extern struct rk_sema sem;
extern pthread_mutex_t mutexList;
extern List *sockets;

void receiveMessage(void *sock_client);  // Reception of a client message
void transmitMessage(void* sock_client); // Sending a message to the client
void sendPrivateMessage(char *msg, int client); // Allows sending a private message

#endif // SERVER_H_