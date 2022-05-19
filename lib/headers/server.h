#ifndef SERVER_H_ /* Include guard */
#define SERVER_H_

#include "sema.h"
#include "list.h"

typedef struct thread_sockets_receive tsr;

struct thread_sockets_receive
{
    int client;
    List *clients;
    rk_sema sem;
    pthread_mutex_t mutex;
};

typedef struct thread_sockets_send tss;

struct thread_sockets_send
{
    int size;
    char *message;
    int client;
    char *pseudoSender;
};

void generateChanel(void *chanel_struct);
void receiveMessage(void *sock_client);
void prepareGenerateChanel(char *name);

#endif // SERVER_H_