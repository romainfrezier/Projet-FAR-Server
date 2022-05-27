/**
 * @file channel.h
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Header for channel and channel list
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef CHANNEL_H_ /* Include guard */
#define CHANNEL_H_

#include <pthread.h>
#include "list.h"
#include "sema.h"

/**
 * @brief Channel data
 */
typedef struct Channel Channel;

/**
 * @brief Channel list data
 */
typedef struct ChannelList ChannelList;

/**
 * @brief Channel data
 *
 * @param port channel port
 * @param name channel name
 * @param theme channel theme
 * @param clients list of clients in the channel
 * @param next next channel in the list
 * @param thread thread of the channel
 * @param channelList list of channel that contain this one
 * @param semaphore semaphore for accept client
 * @param mutex mutex on client list
 */
struct Channel
{
    int port;
    char *name;
    char *theme;
    List *clients;
    Channel *next;
    pthread_t thread;
    ChannelList *channelList;
    rk_sema semaphore;
    pthread_mutex_t mutex;
};

/**
 * @brief Channel list data
 *
 * @param head head of the list
 * @param freePlaces number of channel that we can add
 * @param maxPlaces maximum number of channel in the list
 */
struct ChannelList
{
    Channel *head;
    int freePlaces;
    int maxPlaces;
};

/**
 * @brief Create an empty list of channel with the size 'size'
 *
 * @param size size of the channel list
 * @return An empty list of channel ChannelList*
 */
ChannelList *createChannelList(int size);

/**
 * @brief Add a channel to the list at the last position
 *
 * @param list the list in which we want to add the channel
 * @param channel the channel to add
 */
void addLastChannel(ChannelList *list, Channel *channel);

/**
 * @brief Check if the channel list is empty
 *
 * @param list list of all the channels
 * @return 0 -> list is empty
 * @return 1 -> list is not empty
 */
int channelListIsEmpty(ChannelList *list);

/**
 * @brief Create a new channel
 *
 * @param name name of the new channel
 * @param theme theme of the new channel
 * @param port port of the new channel
 * @param thread thread of the new channel
 * @param size number of client that channel can accept
 * @param semaphore semaphore for accept clients
 * @param mutex mutex on client list
 * @return the new channel created
 */
Channel *createChannel(char *name, char *theme, int port, pthread_t thread, int size, rk_sema semaphore, pthread_mutex_t mutex);

/**
 * @brief List all the channel of the server
 *
 * @param list list of channels in the server
 * @param client id of client who makes the request
 * @return A string of the entire list
 */
char *listChannel(ChannelList *list, int client);

/**
 * @brief Get all the information of a channel with his index on the list
 * @information Index are from 1 to n, like in the channel list string from listChannel function
 *
 * @param list list of channels in the server
 * @param index index of the desired channel
 * @return the channel that correspond to the index
 */
Channel *getChannelByIndex(ChannelList *list, int index);

/**
 * @brief Check if a pseudo is already use in the whole server, through channels
 *
 * @param list list of channels in the server
 * @param pseudo pseudo we want to check
 * @return 0 -> pseudo is in the list
 * @return 1 -> pseudo is not in the list
 */
int pseudoInAllChannel(ChannelList *list, char *pseudo);

/**
 * @brief Remove a channel of the list from its index
 * @information Index are from 1 to n, like in the channel list string from listChannel function
 *
 * @param list list of the channels
 * @param index index of the desired channel
 */
void delChannel(ChannelList *list, int index);

/**
 * @brief Allows the channel to stop and stop all the user connected
 *
 * @param sockets list of connected clients
 * @param sem semaphore that accept clients
 * @param mutexList mutex on the list of clients
 */
void channelQuit(List *sockets, rk_sema sem, pthread_mutex_t mutexList);

/**
 * @brief Get the information of the client message to generate a channel
 *
 * @param cmd client message
 * @return NULL
 */
void *createNewChannel(void *cmd);

/**
 * @brief Check if the client is admin, if the channel has free places and then create a thread for creating a new channel
 *
 * @param clients list of clients in the current channel
 * @param client if of the client who made the request
 * @param freePlaces free places in the channel list
 * @param message request of the client
 */
void checkChannel(List *clients, int client, int freePlaces, char *message);

/**
 * @brief Check if the channel exists, if the client is not already in the channel and then join another channel
 *
 * @param msg request of the client
 * @param channelList list of channels in the server
 * @param client id of the client who made the request
 * @param clients list of clients in the current channel
 */
void joinChannel(char *msg, ChannelList *channelList, int client, List *clients);

/**
 * @brief Check if the client is admin, check the message and then change channel's information
 *
 * @param channelList list of channels in the server
 * @param message message from the client
 * @param client if of the client who made the request
 * @param clients list of clients in the current channel
 */
void modifyChannel(ChannelList *channelList, char *message, int client, List *clients);

/**
 * @brief Check if the client is admin, if the channel exists and then remove a channel of the list
 *
 * @param msg message from the client
 * @param channelList list of channels in the server
 * @param client id of the client who made the request
 * @param clients list of clients in the current channel
 */
void removeChannel(char *msg, ChannelList *channelList, int client, List *clients);

/**
 * @brief Make a list of all the users of the server
 *
 * @param list list of channels in the server
 * @param client id of the client who made the request
 * @param clients list of clients in the current channel
 * @return A string of the entire list
 */
char *getAllUsers(ChannelList *list, int client, List *clients);

#endif // CHANNEL_H_