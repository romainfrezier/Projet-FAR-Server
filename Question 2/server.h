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
};


void receiveMessage(void* sock_client); // Reception of a user message
void transmitMessage(void* sock_client); // Sending a message to the user
void serverQuit(int n); // Allows the server to stop
void userQuit(int socket); // Allows a user to leave the server
void sendPrivateMessage(char *msg); // Allows sending a private message

#endif // SERVER_H_