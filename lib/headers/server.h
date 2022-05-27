/**
 * @file server.h
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Server actions
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SERVER_H_ /* Include guard */
#define SERVER_H_

#include "sema.h"
#include "list.h"
#include "channel.h"

/**
 * @brief Data for the thread that control message reception
 */
typedef struct thread_sockets_receive tsr;

/**
 * @brief Data for the thread that control message reception
 *
 * @param client id of the client who send the message
 * @param clients list of the clients in the channel
 * @param sem semaphore that accept client
 * @param mutex mutex on the list of client
 */
struct thread_sockets_receive
{
    int client;
    List *clients;
    rk_sema sem;
    pthread_mutex_t mutex;
};

/**
 * @brief Data for the thread that control message transmitting
 */
typedef struct thread_sockets_send tss;

/**
 * @brief Data for the thread that control message transmitting
 *
 * @param size size of the message
 * @param message message to send
 * @param client id of the client who will receive the message
 * @param pseudoSender pseudo of the client who send the message
 */
struct thread_sockets_send
{
    int size;
    char *message;
    int client;
    char *pseudoSender;
};

/**
 * @brief Launch the server program
 */
void launchServer();

/**
 * @brief Generate a new channel
 *
 * @param channel_struct channel data
 * @return NULL
 */
void *generateChannel(void *channel_struct);

/**
 * @brief Receive a message from a client
 *
 * @param sock_client Data of the client message
 * @return NULL
 */
void *receiveMessage(void *sock_client);

/**
 * @brief Prepare to generate a new channel
 *
 * @param name name of the new channel
 * @param theme theme of the new channel
 */
void prepareGenerateChannel(char *name, char *theme);

/**
 * @brief Create a socket and bind it
 *
 * @param port port of the socket
 * @return the socket descriptor (dS)
 */
int createAndBindSocket(int port);

/**
 * @brief Turn off the server
 *
 * @param n signal int
 */
void serverQuit(int n);

/**
 * @brief Send a message to all the clients of the server
 *
 * @param signal signal number
 */
void sendMessageForAllUsers(int signal);

/**
 * @brief Signal handler for kick a user through channel
 *
 * @param channels list of server channels
 * @param pseudo pseudo of user kicked
 */
void kickUserFromServer(int signal);

#endif // SERVER_H_