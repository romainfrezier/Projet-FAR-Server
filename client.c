#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#define TAILLE 100

int main(int argc, char *argv[]) {


  if (argc != 3){
    printf("Nommbre d'argument insuffisant -> Adresse serveur + port\n");
    exit(0);
  }


  printf("Début programme\n");
  //PF_INET = IPV4 , SOCK_STREAM = TCP
  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket Créé\n");

  struct sockaddr_in aS;
  aS.sin_family = AF_INET;

  //converti l'adresse ip fourni en ipv4
   if (inet_pton(AF_INET,argv[1],&(aS.sin_addr)) == -1){
     printf("erreur socket\n");
     perror("erreur");
     exit(0);
    }
  //
  aS.sin_port = htons(atoi(argv[2]));
  socklen_t lgA = sizeof(struct sockaddr_in);

  if (connect(dS, (struct sockaddr *) &aS, lgA) == -1){
    printf("erreur connexion socket\n");
    perror("erreur");
    exit(0);
  }
  printf("Socket Connecté\n");



  while(1){
      char* texte = (char*)malloc(TAILLE);
      printf("Choisissez un texte à envoyer :\n");
      scanf("%s",texte);
      int taille = strlen(texte);
      

      printf("la taille choisie est %d \n", taille);

    //envoi d'un message au serveur
      if (send(dS, &taille, sizeof(int), 0) == -1){
        printf("erreur envoi taille serveur\n");
        perror("erreur");
        exit(0);
      }
      printf("Taille du message envoyée\n");



      if (send(dS, texte, strlen(texte) , 0) == -1){
        printf("erreur envoi message serveur\n");
        perror("erreur");
        exit(0);
      }
      printf("Message Envoyé \n");

      u_long r_taille;

      if(recv(dS, &r_taille, sizeof(u_long), 0) == -1){
        printf("erreur reception message validation taille serveur\n");
        perror("erreur");
        exit(0);
      }

    //reception d'un message du serveur
      char* r = (char*)malloc(r_taille*sizeof(char));

      if (recv(dS, &r, r_taille, 0) == -1){
        printf("erreur reception message du serveur\n");
        perror("erreur");
        exit(0);
      }
      printf("Réponse reçue : %d\n", r) ;
  }

  

//fermeture du socket
  shutdown(dS,2) ;
  printf("Fin du programme");
}