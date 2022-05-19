#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "../headers/chanel.h"
#include "../headers/list.h"

Chanel* createChanel(char* name, int port, pthread_t thread){
    Chanel* newChanel = (Chanel*)malloc(sizeof(Chanel));
    List* sockets = (List*)malloc(sizeof(List));
    newChanel->name = name;
    newChanel->port = port;
    newChanel->clients = sockets;
    newChanel->thread = thread;
    newChanel->next = NULL;
    return newChanel;
}

