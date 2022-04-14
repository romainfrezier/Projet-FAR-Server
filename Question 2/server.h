#ifndef SERVER_H_ /* Include guard */
#define SERVER_H_


typedef struct thread_sockets_receive tsr;

struct thread_sockets_receive
{
    int client;
    int **clients;
    int *nb_client;
};

typedef struct thread_sockets_send tss;

struct thread_sockets_send
{
    int size;
    char *message;
    int client;
    int *nb_client;
};


void receiveMessage(void* sock_client); // Reception d'un message du client

void sendMessage(void* sock_client); // Envoi d'un message au client


#endif // SERVER_H_