#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "../headers/channel.h"
#include "../headers/list.h"

Channel* createChannel(char* name, int port, pthread_t thread){
    Channel* newChannel = (Channel*)malloc(sizeof(Channel));
    List* sockets = (List*)malloc(sizeof(List));
    newChannel->name = name;
    newChannel->port = port;
    newChannel->clients = sockets;
    newChannel->thread = thread;
    newChannel->next = NULL;
    return newChannel;
}

