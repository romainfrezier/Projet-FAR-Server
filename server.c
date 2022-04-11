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
  int connection = 1;
  int sockets[2];
  while (connection <= 2){
      sockets[connection-1] = accept(dS, (struct sockaddr*) &aC, &lg);
      printf("Client %d Connecté\n", connection);
      connection += 1;
  }
  char *msg;
  char* ok = "ok";
  u_long taille;
  taille = strlen(ok);
  int res_size1 = send(sockets[0], &taille, sizeof(u_long), 0);
  if (res_size1 == -1)
  {
      perror("Erreur envoi taille");
  }
  int res_role = send(sockets[0], ok, taille*sizeof(char), 0);
  if (res_role == -1)
  {
      perror("Erreur envoi tour");
  }
  while (strcmp(msg,"quit\n")!=0)
  {
    recv(sockets[0], &taille, sizeof(u_long),0);

    char* msg = (char*)malloc(taille);
    recv(sockets[0], msg, taille, 0);
    printf("Message reçu : %s", msg) ;
    
    
    int res_size2 = send(sockets[1], &taille, sizeof(u_long), 0);
    if (res_size2 == -1)
    {
      perror("Erreur envoi taille");
    }

    int res_msg = send(sockets[1], msg, taille, 0);
    if (res_msg == -1)
    {
      perror("Erreur envoi message");
    }
    printf("Message Envoyé\n");
    free(msg);
    
  }
  shutdown(sockets[0], 2);
  shutdown(sockets[1], 2);
  shutdown(dS, 2) ;
  printf("Fin du programme \n");
}