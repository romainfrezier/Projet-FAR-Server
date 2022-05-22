#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../headers/channel.h"

Channel* createChannel(char* name, int port, pthread_t thread, int size){
    Channel* newChannel = (Channel*)malloc(sizeof(Channel));
    List* sockets = createList(size);
    newChannel->name = name;
    newChannel->port = port;
    newChannel->clients = sockets;
    newChannel->thread = thread;
    newChannel->next = NULL;
    return newChannel;
}

