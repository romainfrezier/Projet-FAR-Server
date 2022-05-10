#ifndef CLIENT_H_   /* Include guard */
#define CLIENT_H_

typedef struct sendFileStruct sendFileStruct;

struct sendFileStruct
{
    int socketServer;
    char* fileName;
    int port;
};

void receiveMessage(int socket);  // Reception of a server message
void sendMessage(int socket); // Sending a message to the server
void sendSpecificMessage(int client, char *message); // Sending a specific message to the server
void displayManuel(); // Display the commands manual
void quitForUser(int n); // Allows User n to leave the server
int receivePort(int socket); // receive a port from the server
void sendFile(void* data); // send a file to the server in a different socket


#endif // CLIENT_H_
