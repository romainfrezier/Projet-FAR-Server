/**
 * @file commandServer.h
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Command management functions
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef COMMANDSERVER_H_ /* Include guard */
#define COMMANDSERVER_H_

#include "sema.h"
#include "server.h"
#include "channel.h"

/**
 * @brief Send a message to the client
 *
 * @param client : id of the client who will receive the message
 * @param message : message to send
 */
void sendSpecificMessage(int client, char *message);

/**
 * @brief Check which command is made by the client
 *
 * @param msg : message from the client
 * @param sock_cli : list of the clients in the current channel
 * @param sem : semaphore that accept clients
 * @param channelList : list of channels in the server
 * @return -1 -> the user make a /quit
 * @return 0 -> it's ok
 */
int checkCommand(char *msg, tsr *sock_cli, rk_sema sem, ChannelList *channelList);

/**
 * @brief Allows a user to leave the server
 *
 * @param socket : id of the client who made the request
 * @param sockets : list of clients in the current channel
 * @param sem : semaphore that accept clients
 * @param mutex : mutex on the clients list
 */
void userQuit(int socket, List *sockets, rk_sema sem, pthread_mutex_t mutex);

/**
 * @brief Check if a message contains insult
 *
 * @param message : message of the client
 * @return 0 -> there is no insult
 * @return 1 -> there is insult
 */
int checkCensorship(char *message);

/**
 * @brief Change the username of the client
 *
 * @param msg : message from the client
 * @param client : id of the client who made the request
 * @param sockets : list of the clients in the current channel
 */
void renameUser(char *msg, int client, List *sockets);

/**
 * @brief Send a private message to the client
 *
 * @param msg : message to send
 * @param client : id of the client who made the request
 * @param sockets : list of clients in the current channel
 */
void sendPrivateMessage(char *msg, int client, List *sockets);

/**
 * @brief Transmit a message to one client
 *
 * @param sock_client : structure that contains the message and the id of the receiver
 * @return NULL
 */
void * transmitMessage(void *sock_client);

/**
 * @brief Send a number to the client
 *
 * @param client : id of the client who will receive the message
 * @param number : number to send
 */
void sendSpecificNumber(int client, int number);

/**
 * @brief Check if user is admin before send message to all users
 *
 * @param msg : message to send
 * @param channelList : list of all the channels in the server
 * @param clients : list of clients in the current channel
 * @param client : id of client who made the request
 */
void sendAllMessage(char* msg, ChannelList* channelList, List* clients, int client);

/**
 * @brief Send the list of users of the whole server, and warn client if he is not an admin
 *
 * @param list : list of all the channels in the server
 * @param client : id of client who made the request
 * @param clients : list of clients in the current channel
 */
void displayAllUsers(ChannelList *list, int client, List *clients);

/**
 * @brief Warn all the user that the server will stop and then turn off the server
 *
 * @param channelList : list of all the channels in the server
 * @param clients : list of clients in the current channel
 * @param client : client who made the request
 */
void shutdownServer(ChannelList* channelList, List* clients, int client);

/**
 * @brief Check if the user is admin and then add one word to the censorship words list
 *
 * @param message : message from the client
 * @param clients : list of clients in the current channel
 * @param client : id of the client who made the request
 */
void addWord(char *message, List* clients, int client);

#endif // COMMANDSERVER_H_