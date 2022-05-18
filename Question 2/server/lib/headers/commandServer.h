#ifndef COMMANDSERVER_H_ /* Include guard */
#define COMMANDSERVER_H_

void sendSpecificMessage(int client, char *message); // Send a specific message to client selected
int checkCommand(char *msg, tsr *sock_cli);          // check wich command the user give
void serverQuit(int n);                              // Allows the server to stop and stop all the user connected
void userQuit(int socket);                           // Allows a user to leave the server
void displayAllUsers(int client);                    // send a message of all user of the chat server
int checkCensorship(char *message);                  // Check if a message contains insult. Return 0 if there is no insult
void renameUser(char *msg, int client);              // change user username

#endif // COMMANDSERVER_H_