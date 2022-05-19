#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "lib/headers/server.h"
#include "lib/headers/sema.h"
#include "lib/headers/admin.h"
#include "lib/headers/chanel.h"

ChanelList* chanelList;
int maxChanel = 10;



// We want to create a send thread and a reception thread for each user
int main(int argc, char *argv[])
{
  chanelList = createChanelList(maxChanel);

  //tout mettre dans la fonction generateChanel et faire cette fonction dans le fichier server.c (gros merdier)
  Chanel* firstChanel = createChanel("Welcome", argv[1]);
  generateChanel(atoi(argv[1]));
  addFirstChanel(chanelList, firstChanel);


  // Server shutdown
  free(adminKey);
  printf("End program\n");
}