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

typedef struct thread_receiveFile trf;

struct thread_receiveFile {
    char* fileName;
    int client;
    long fileSize;
};

typedef struct fileStruct fileStruct;

struct fileStruct
{
    size_t filenameSize;
    char *filename;
    long fileSize;
};

void receiveMessage(void* sock_client); // Reception of a user message
void transmitMessage(void* sock_client); // Sending a message to the user
void serverQuit(int n); // Allows the server to stop
void userQuit(int socket); // Allows a user to leave the server
void sendPrivateMessage(char *msg, int client); // Allows sending a private message
void sendSpecificMessage(int client, char *message); // Send a specific message to user selected
void executeError(char *errorMessage); // Display an error
void adminVerification(char* message, int client); // check the key that the user send to the server and put him admin if the key is true
void kick(char* message, int client); // kick a user from the server
void displayAllUsers(int client); // display all the users connected for the user
void generateAdminKey(char* key); // generate an admin key
void fileThreadFunc();
void receiveFile(fileStruct *fileInfo, int client);
void fileTransferReception(void *receiveFileData);
#endif // SERVER_H_