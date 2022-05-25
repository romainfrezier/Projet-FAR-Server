#ifndef ADMIN_H_ /* Include guard */
#define ADMIN_H_

#include "list.h"
#include "channel.h"

extern char *adminKey;
extern char allMessage[100];

void generateAdminKey(char *key);                                 // generate a random admin key
void kick(char *message, int client, List *sockets, rk_sema semaphore, pthread_mutex_t mutex);              // remove a user from the chat server
void adminVerification(char *message, int client, List *sockets); // check the password give by a user
void sendAllUsersMessage(ChannelList *channels, char *message);

#endif // ADMIN_H_