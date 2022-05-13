#ifndef COMMANDSERVER_H_ /* Include guard */
#define COMMANDSERVER_H_

int checkCommand(char *msg, tsr *sock_cli);
void serverQuit(int n);
void userQuit(int socket);
void displayAllUsers(int client);

#endif // COMMANDSERVER_H_