#ifndef FILESERVER_H_ /* Include guard */
#define FILESERVER_H_

void fileTransferReception(int socket, char *filename);
void receiveFile(char *msg, int client);
void receiveFileThread(void *receiveFileData, int dSfile);

#endif // FILESERVER_H_