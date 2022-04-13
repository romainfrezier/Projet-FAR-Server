#ifndef CLIENT_H_   /* Include guard */
#define CLIENT_H_

void receiveMessage(int socket);  //Reception d'un message du serveur

void sendMessage(int socket); //Envoi d'un message au serveur

#endif // CLIENT_H_
