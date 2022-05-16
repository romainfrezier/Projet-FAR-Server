#ifndef COMMANDSERVER_H_ /* Include guard */
#define COMMANDSERVER_H_

int checkCommand(char *msg, tsr *sock_cli); // check wich command the user give
void serverQuit(int n);                     // Allows the server to stop and stop all the user connected
void userQuit(int socket);                  // Allows a user to leave the server
void displayAllUsers(int client);           // send a message of all user of the chat server

#endif // COMMANDSERVER_H_