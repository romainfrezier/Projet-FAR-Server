#ifndef CHANNEL_H_ /* Include guard */
#define CHANNEL_H_

#include <pthread.h>
#include "list.h"

typedef struct Channel Channel;
typedef struct ChannelList ChannelList;

struct Channel {
    int port;
    char* name;
    List* clients;
    Channel* next;
    pthread_t thread;
    ChannelList *channelList;
};

struct ChannelList
{
    Channel *head;
    int freePlaces;
    int maxPlaces;
};

ChannelList *createChannelList(int size);
void addLastChannel(ChannelList *list, Channel *channel);
int channelListIsEmpty(ChannelList *list);
Channel *createChannel(char *name, int port, pthread_t thread, int size);
char *listChannel(ChannelList *list);
Channel *getChannelByIndex(ChannelList *list, int index);
int pseudoInAllChannel(ChannelList *list, char *pseudo);

#endif // CHANNEL_H_