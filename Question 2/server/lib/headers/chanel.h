#ifndef CHANEL_H_ /* Include guard */
#define CHANEL_H_

#include "list.h"

extern struct rk_sema sem;
extern pthread_mutex_t mutexList;
extern int dS;

typedef struct thread_sockets_receive tsr;

struct thread_sockets_receive
{
    int client;
    List *clients;
    rk_sema sem;
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

#endif // CHANEL_H_