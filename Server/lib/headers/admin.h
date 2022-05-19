#ifndef ADMIN_H_ /* Include guard */
#define ADMIN_H_

#include "list.h"

extern char *adminKey;

void generateAdminKey(char *key);                   // generate a random admin key
void kick(char *message, int client, List *sockets); // remove a user from the chat server
void adminVerification(char *message, int client, List* sockets);  // check the password give by a user

#endif // ADMIN_H_