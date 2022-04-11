#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  if (argc!=3) {
    perror("Utilisation : ./exe adresseIP port");
  }

  if (atoi(argv[2])<=1024) {
    perror("Port incorrect : doit être supérieur à 1024");
  }

  printf("Début programme\n");
  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket Créé\n");

  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
  aS.sin_port = htons(atoi(argv[2])) ;
  socklen_t lgA = sizeof(struct sockaddr_in) ;
  connect(dS, (struct sockaddr *) &aS, lgA) ;
  printf("Socket Connecté\n");

  char *m;
  while (strcmp(m, "quit\n") != 0)
  {
    u_long size;
    if(recv(dS, &size, sizeof(u_long), 0) == -1){
        perror("Erreur Taille message reçu\n");
        exit(0);
    } 
    printf("Taille reçu : %lu\n", size);
    char* res = (char*)malloc(size*sizeof(char));
    if(recv(dS, &res, size*sizeof(char), 0) == -1){
        perror("Erreur message reçu\n");
        exit(0);
    } 
    printf("Message reçu : %s\n", res);
    printf("Entrez votre message ('quit' pour quitter) : ");
    fgets(m, 50, stdin);
    u_long taille = strlen(m)+1;
    printf("Taille message : %lu\n", taille);
    send(dS, &taille, sizeof(u_long), 0);
    send(dS, m, taille, 0);
    printf("Message Envoyé \n");

    u_long r;
    recv(dS, &r, sizeof(u_long), 0) ;
    printf("Réponse reçue : %lu\n", r) ;
  }
  
  shutdown(dS,2) ;
  printf("Fin du programme \n");
}