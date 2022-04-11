#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

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

  listen(dS, 1) ;
  printf("Mode écoute\n");

  struct sockaddr_in aC ;
  socklen_t lg = sizeof(struct sockaddr_in) ;
  int connection = 0;
  int maxConnection = 2;
  int sockets[2];
  while (connection < maxConnection){
      sockets[connection] = accept(dS, (struct sockaddr*) &aC, &lg);
      printf("Client %d Connecté\n", connection+1);
      connection += 1;
  }
  char *msg;
  char* ok = "ok";
  u_long taille;
  taille = strlen(ok);
  if (send(sockets[0], &taille, sizeof(u_long), 0) == -1)
  {
      perror("Erreur envoi taille\n");
      exit(0);
  }
  if (send(sockets[0], ok, taille*sizeof(char), 0) == -1)
  {
      perror("Erreur envoi tour\n");
      exit(0);
  }

  int client = 0;

  while (strcmp(msg,"quit\n")!=0)
  {
    if(recv(sockets[client%maxConnection], &taille, sizeof(u_long),0) == -1){
      perror("Erreur recep taille\n");
      exit(0);
    }
    printf("Val : %d\n", client%maxConnection);
    char* msg = (char*)malloc(taille);
    if(recv(sockets[client%maxConnection], msg, taille, 0) == -1){
      perror("Erreur recep msg\n");
      exit(0);
    }
    printf("Message reçu : %s", msg) ;
    
    
    if (send(sockets[(client+1)%maxConnection], &taille, sizeof(u_long), 0) == -1)
    {
      perror("Erreur envoi taille\n");
    }

    if (send(sockets[(client+1)%maxConnection], msg, taille, 0) == -1)
    {
      perror("Erreur envoi message\n");
    }
    printf("Message Envoyé\n");
    free(msg);
    client += 1;
  }
  shutdown(sockets[0], 2);
  shutdown(sockets[1], 2);
  shutdown(dS, 2) ;
  printf("Fin du programme \n");
}