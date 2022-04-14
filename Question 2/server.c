#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include <pthread.h>

#define MAX_CONNECTION 5

int main(int argc, char *argv[]) {

  if (argc!=2) {
    printf("mauvaise utilisation : manque d'arg");
    printf("utilisation : ./exe port");
    exit(0);
  }

  if (atoi(argv[1]) <= 1024)
  {
    perror("Port incorrect : doit être supérieur à 1024");
  }

  printf("Début programme\n");


  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket Créé\n");


  struct sockaddr_in ad;
  ad.sin_family = AF_INET;
  ad.sin_addr.s_addr = INADDR_ANY ;
  ad.sin_port = htons(atoi(argv[1])) ;
  bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
  printf("Socket Nommé\n");

  listen(dS, MAX_CONNECTION) ;
  printf("Mode écoute\n");

  struct sockaddr_in aC ;
  socklen_t lg = sizeof(struct sockaddr_in) ;
  int connection = 0;
  int sockets[MAX_CONNECTION];

  while (1)
  {
    while (connection < MAX_CONNECTION){
      int acceptation = accept(dS, (struct sockaddr *)&aC, &lg);
      sockets[connection] = acceptation;
      printf("Client %d Connecté\n", connection + 1);
      printf("idclientconnecté = %d\n", sockets[connection]);
      tsr *receiveData = (tsr *)malloc(sizeof(tsr));
      (*receiveData).client = sockets[connection];
      (*receiveData).clients = sockets;
      pthread_t receive;
      printf("idclientconnecté2 = %d\n", (*receiveData).client);
      pthread_create(&receive, NULL, receiveMessage, (void *)receiveData);
      connection += 1;
      // voir si on free
    }
  }
  //shutdown de toutes les sockets
  for (int i = 0; i < sizeof(sockets); i++){
    shutdown(sockets[i], 2);
  }
  shutdown(dS, 2);
  printf("Fin du programme \n");
}

void sendMessage(void* sock_client){
  tss* sock_cli = (tss*)sock_client;
  if (send((*sock_cli).client, &((*sock_cli).size), sizeof(u_long), 0) == -1)
  {
    perror("Erreur envoi taille\n");
  }

  if (send((*sock_cli).client, (*sock_cli).message, (*sock_cli).size, 0) == -1)
  {
    perror("Erreur envoi message\n");
  }
  printf("Message Envoyé\n");
}

void receiveMessage(void* sock_client){
    tsr* sock_cli = (tsr*)sock_client;
    char *msg;
    u_long taille;
    printf("testreceive idclient : %d", (*sock_cli).client);
    while(1){
      if(recv((*sock_cli).client, &taille, sizeof(u_long),0) == -1){
        perror("Erreur recep taille\n");
        exit(0);
      }
      char* msg = (char*)malloc(taille);
      if(recv((*sock_cli).client, msg, taille, 0) == -1){
        perror("Erreur recep msg\n");
        exit(0);
      }
      printf("Message reçu : %s", msg);
      pthread_t send;
      size_t array_size = (sizeof((*sock_cli).clients)) / (sizeof(*sock_cli).clients[0]); 
      printf("taille tableau sockets %d\n", array_size);
      for (int i = 0; i < array_size; i++){
        tss* sendData = (tss*)malloc(sizeof(tss));
        (*sendData).client = (*sock_cli).clients[i];
        (*sendData).size = taille;
        (*sendData).message = msg;
        //exécution des threads
        printf("id client %d : : %d\n", i, (*sendData).client);
        pthread_create(&send, NULL, sendMessage, (void*) sendData);
      }
    }
    free(msg);
}