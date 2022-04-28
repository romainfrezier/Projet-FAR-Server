#include "stringFunc.h"
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

#include "list.h"
#include "server.h"
// #include "command.h"

unsigned int MAX_CONNECTION = 3;
List *sockets;

struct rk_sema {
  #ifdef __APPLE__
      dispatch_semaphore_t    sem;
  #else
      sem_t                   sem;
  #endif
};

static inline void rk_sema_init(struct rk_sema *s, uint32_t value) {
  #ifdef __APPLE__
      dispatch_semaphore_t *sem = &s->sem;

      *sem = dispatch_semaphore_create(value);
  #else
      sem_init(&s->sem, 0, value);
  #endif
}

static inline void rk_sema_wait(struct rk_sema *s) {
  #ifdef __APPLE__
      dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
  #else
      int r;
      do {
        r = sem_wait(&s->sem);
      } while (r == -1 && errno == EINTR);
  #endif
}

static inline void rk_sema_post(struct rk_sema *s) {
  #ifdef __APPLE__
      dispatch_semaphore_signal(s->sem);
  #else
      sem_post(&s->sem);
  #endif
}

static inline void rk_sema_destroy(struct rk_sema *s) {
  #ifdef __APPLE__
      dispatch_release(s->sem);
  #else
      sem_destroy(&s->sem);
  #endif
}

struct rk_sema sem;
struct rk_sema semList;


// Functions to print in color
// Green for transmitting message
void green () {
  printf("\033[0;32m");
}
// Blue for receiving message
void blue () {
  printf("\033[0;34m");
}
// Purple for receiving private message
void purple () {
  printf("\033[0;35m");
}
// Red for errors
void red () {
  printf("\033[0;31m");
}
// Reset color to white
void reset () {
  printf("\033[0m");
}

// We want to create a send thread and a recption thread for each user
int main(int argc, char *argv[])
{

  // Definition of the socket array to the desired size
  sockets = createList(MAX_CONNECTION);

  rk_sema_init(&sem, MAX_CONNECTION);
  rk_sema_init(&semList,1);
  if (argc != 2)
  {
    red();
    printf("Usage : ./exe port");
    reset();
    exit(0);
  }

  if (atoi(argv[1]) <= 1024)
  {
    red();
    perror("Bad port: must be greater than 1024");
    reset();
  }

  printf("Start program\n");

  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket created\n");

  struct sockaddr_in ad;
  ad.sin_family = AF_INET;
  ad.sin_addr.s_addr = INADDR_ANY;
  ad.sin_port = htons(atoi(argv[1]));
  bind(dS, (struct sockaddr *)&ad, sizeof(ad));
  printf("Socket named\n");

  listen(dS, MAX_CONNECTION);
  printf("Listening mode on port %d \n", atoi(argv[1]));

  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);
  signal(SIGINT, serverQuit);
  while (1)
  {
    // Users are accepted until max allow
    // Wait for space in the user list
    rk_sema_wait(&sem);
    int acceptation = accept(dS, (struct sockaddr *)&aC, &lg);

    char* coMsg = "Please choose your username : ";
    u_long sizeCoMsg;
    sizeCoMsg = strlen(coMsg) + 1;
    // Send connection message size
    if (send(acceptation, &sizeCoMsg, sizeof(u_long), 0) == -1)
    {
      red();
      printf("Error sending size for socket : %d\n", acceptation);
      reset();
      exit(0);
    }
    // Send connection message
    if(send(acceptation, coMsg, sizeCoMsg, 0) == -1)
    {
      red();
      perror("Error sending connection message\n");
      reset();
      exit(0);
    }

    // and we attributed a reception thread to each
    tsr *receiveData = (tsr *)malloc(sizeof(tsr));
    (*receiveData).client = acceptation;
    (*receiveData).clients = sockets;
    pthread_t receive;
    pthread_create(&receive, NULL, receiveMessage, (void *)receiveData);

    // Maybe free here
    
  }
  // Shutdown of all user sockets
  Link *current = sockets->head;
  do
  {
    delVal(sockets, current->value);
    shutdown(current->value, 2);
    current = current->next;
  } while (current->next != NULL);

  // Server shutdown
  shutdown(dS, 2);
  rk_sema_destroy(&semList);
  rk_sema_destroy(&sem);
  printf("End program\n");
}

void transmitMessage(void *sock_client)
{
  tss *sock_cli = (tss *)sock_client;

  // Transmit message size
  if (send((*sock_cli).client, &((*sock_cli).size), sizeof(u_long), 0) == -1)
  {
    red();
    printf("Error sending size for socket : %d\n", (*sock_cli).client);
    reset();
    exit(0);
  }

  // Transmit message size
  if (send((*sock_cli).client, (*sock_cli).message, (*sock_cli).size, 0) == -1)
  {
    red();
    perror("Error sending message\n");
    reset();
    exit(0);
  }
  green();
  printf("Message transmitted\n");
  reset();
}

void receiveMessage(void *sock_client)
{
  tsr *sock_cli = (tsr *)sock_client;
  char *msg;
  u_long size;
  char* pseudo;
  int check;
  do{
    if (recv((*sock_cli).client, &size, sizeof(u_long), 0) == -1)
    {
      red();
      perror("Error message size received\n");
      reset();
      exit(0);
    }
    blue();
    printf("Size received : %lu\n", size);
    reset();
    pseudo = (char*)malloc(sizeof(char)*size);
    if (recv((*sock_cli).client, pseudo, size, 0) == -1)
    {
      red();
      perror("Error message received\n");
      reset();
      exit(0);
    }
    blue();
    printf("Pseudo received : %s\n", pseudo);
    reset();
    check = pseudoInList(sockets, pseudo);
    printf("check : %d\n",check);
    if (check == 0){
      char* coMsg = "Username already used !\nPlease choose another username : ";
      u_long sizeCoMsg;
      sizeCoMsg = strlen(coMsg) + 1;
      // Send connection message size
      if (send((*sock_cli).client, &sizeCoMsg, sizeof(u_long), 0) == -1)
      {
        red();
        printf("Error sending size for socket : %d\n", (*sock_cli).client);
        reset();
        exit(0);
      }
      // Send connection message
      if(send((*sock_cli).client, coMsg, sizeCoMsg, 0) == -1)
      {
        red();
        perror("Error sending connection message\n");
        reset();
        exit(0);
      }
    }
  }while(check == 0);

  char* connectionValidate = "Connected !";
  u_long sizeCoMsg;
  sizeCoMsg = strlen(connectionValidate) + 1;
  // Send connection message size
  if (send((*sock_cli).client, &sizeCoMsg, sizeof(u_long), 0) == -1)
  {
    red();
    printf("Error sending size for socket : %d\n", (*sock_cli).client);
    reset();
    exit(0);
  }
  // Send connection message
  if(send((*sock_cli).client, connectionValidate, sizeCoMsg, 0) == -1)
  {
    red();
    perror("Error sending connection message\n");
    reset();
    exit(0);
  }

  blue();
  printf("Pseudo received : %s\n", pseudo);
  reset();
  //add the client to the socket list
  rk_sema_wait(&semList);
  addFirst(sockets, sock_cli->client, pseudo);
  rk_sema_post(&semList);
  printf("User connected with id : %d\n", sock_cli->client);

  while (1)
  {
    // Size reception
    if (recv((*sock_cli).client, &size, sizeof(u_long), 0) == -1)
    {
      red();
      perror("Error message size received\n");
      reset();
      exit(0);
    }
    blue();
    printf("Size received : %lu\n", size);
    reset();

    // Message reception
    char *msg = (char *)malloc(size);
    if (recv((*sock_cli).client, msg, size, 0) == -1)
    {
      red();
      perror("Error message received\n");
      reset();
      exit(0);
    }
    blue();
    printf("Message received : %s\n", msg);
    reset();

    // Commands management here
    if (msg[0] == '/')
    {
      printf("Command detected\n");
      char* copyMessage = (char*)malloc(strlen(msg)+1);
      strcpy(copyMessage,msg);
      char *strto = strtok(copyMessage, " ");
      if (strcmp(msg, "/quit") == 0)
      {
        printf("ici 1 : %d\n",(*sock_cli).client);
        userQuit((*sock_cli).client);
        break;
      }
      else if (strcmp(strto, "/mp") == 0)
      {
        printf("go to private message\n");
        sendPrivateMessage(msg);
      }
    }
    else
    {
      // Send to each user
      pthread_t send;
      Link *current = sockets->head;
      displayList(sockets);
      for (int i = 0; i < MAX_CONNECTION - sockets->size; i++)
      {
        printf("i = %d\n", i);
        printf("(*sock_cli).client = %d\n", (*sock_cli).client);
        printf("current->value = %d\n", current->value);
        printf("sockets->size = %d\n", sockets->size);
        if (current->value != (*sock_cli).client)
        {
          tss *sendData = (tss *)malloc(sizeof(tss));
          (*sendData).client = current->value;
          (*sendData).size = size;
          (*sendData).message = msg;

          // Execution of threads
          pthread_create(&send, NULL, transmitMessage, (void *)sendData);
        }
        current = current->next;
      }
    }
  }
  free(msg);
}

// Allows the server to stop
void serverQuit(int n)
{
  // Shutdown of all user sockets
  Link *current = sockets->head;
  printf(" value du current : %d\n", current->value);
  while (current != NULL)
  {
    Link *next = current->next;
    shutdown(current->value, 2);
    printf("User %d has been stopped\n", current->value);
    rk_sema_wait(&semList);
    delVal(sockets, current->value);
    rk_sema_post(&semList);
    current = next;
  }
  printf("The server has been stopped !\n");
  exit(0);
}

// Allows a user to leave the server
void userQuit(int socket)
{
  rk_sema_wait(&semList);
  delVal(sockets, socket);
  rk_sema_post(&semList);
  shutdown(socket, 2);
  rk_sema_post(&sem);
}

// Allows sending a private message
void sendPrivateMessage(char *msg)
{
  char** mess = str_split(msg);
  char* cmd = (char*)malloc(strlen(mess[0]));
  cmd = mess[0];
  int target = atoi(mess[1]);
  int commandSize = sizeof(cmd);
  int idSize = sizeof(target);
  tss* sendData = (tss*)malloc(sizeof(tss));
  (*sendData).client = target;
  (*sendData).size = strlen(mess[2]);
  (*sendData).message = mess[2];
  pthread_t send;
  pthread_create(&send, NULL, transmitMessage, (void *)sendData);
}