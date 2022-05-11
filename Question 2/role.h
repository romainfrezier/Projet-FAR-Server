#ifndef ROLE_H_ /* Include guard */
#define ROLE_H_

void adminVerification(char *message, int client, List *sockets); // check the key that the user send to the server and put him admin if the key is true
void kick(char *message, int client, List *sockets);              // kick a user from the server
void generateAdminKey(char *key);                  // generate an admin key

#endif // ROLE_H_