/**
 * @file admin.h
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Admin functions
 * @version 0.1
 * @date 2022-05-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef ADMIN_H_ /* Include guard */
#define ADMIN_H_

#include "list.h"
#include "channel.h"

/**
 * @brief global var for the password for admin
 */
extern char *adminKey;

/**
 * @brief global var for the message through different channel
 */
extern char allMessage[100];

/**
 * @brief Generate a random admin password for the server
 *
 * @return A string of the password
 */
char* generateAdminKey();

/**
 * @brief Remove a user from the chat server
 *
 * @param message message from the client
 * @param client id of the client who made the request
 * @param sockets list of clients in the current channel
 * @param semaphore semaphore that accept client in the current channel
 * @param mutex mutex on the clients list
 */
void kick(char *message, int client, List *sockets, rk_sema semaphore, pthread_mutex_t mutex);

/**
 * @brief Check the password give by a user
 *
 * @param message message from the client
 * @param client id of the client who made the request
 * @param sockets list of clients in the current channel
 */
void adminVerification(char *message, int client, List *sockets);

/**
 * @brief Send a signal to all the channel of the server, then they will send a message to their clients
 *
 * @param channels list of all the channels in the server
 * @param message message from the client
 */
void sendAllUsersMessage(ChannelList *channels, char *message);

#endif // ADMIN_H_