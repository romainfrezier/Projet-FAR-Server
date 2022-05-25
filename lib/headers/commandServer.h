#ifndef COMMANDSERVER_H_ /* Include guard */
#define COMMANDSERVER_H_

#include "sema.h"
#include "server.h"
#include "channel.h"

void sendSpecificMessage(int client, char *message);                          // Send a specific message to client selected
int checkCommand(char *msg, tsr *sock_cli, rk_sema sem, ChannelList *channelList); // check wich command the user give
void userQuit(int socket, List *sockets, rk_sema sem, pthread_mutex_t mutex); // Allows a user to leave the server
int checkCensorship(char *message);                                           // Check if a message contains insult. Return 0 if there is no insult
void renameUser(char *msg, int client, List *sockets);                        // change user username
void sendPrivateMessage(char *msg, int client, List *sockets);
void * transmitMessage(void *sock_client);
void sendSpecificNumber(int client, int number);
void sendAllMessage(char* msg, ChannelList* channelList, List* clients, int client);
void displayAllUsers(ChannelList *list, int client, List *clients);
void shutdownServer(ChannelList* channelList, List* clients, int client);

#endif // COMMANDSERVER_H_