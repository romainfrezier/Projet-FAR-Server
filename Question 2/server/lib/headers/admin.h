#ifndef ADMIN_H_ /* Include guard */
#define ADMIN_H_

extern char *adminKey;

void generateAdminKey(char *key);                   // generate a random admin key
void kick(char *message, int client);               // remove a user from the chat server
void adminVerification(char *message, int client);  // check the password give by a user

#endif // ADMIN_H_