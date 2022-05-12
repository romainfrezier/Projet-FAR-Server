#ifndef ADMIN_H_ /* Include guard */
#define ADMIN_H_

extern char *adminKey;

void generateAdminKey(char *key);
void kick(char *message, int client);
void adminVerification(char *message, int client);

#endif // ADMIN_H_