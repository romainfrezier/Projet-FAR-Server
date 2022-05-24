#ifndef CHANNEL_H_ /* Include guard */
#define CHANNEL_H_

#include <pthread.h>
#include "list.h"
#include "sema.h"

typedef struct Channel Channel;
typedef struct ChannelList ChannelList;

struct Channel {
    int port;
    char* name;
    List* clients;
    Channel* next;
    pthread_t thread;
    ChannelList *channelList;
    rk_sema semaphore;
    pthread_mutex_t mutex;
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
Channel *createChannel(char *name, int port, pthread_t thread, int size, rk_sema semaphore, pthread_mutex_t mutex);
char *listChannel(ChannelList *list, int client);
Channel *getChannelByIndex(ChannelList *list, int index);
int pseudoInAllChannel(ChannelList *list, char *pseudo);
void delChannel(ChannelList *list, int index);
void channelQuit(List *sockets, rk_sema sem, pthread_mutex_t mutexList); // Allows the server to stop and stop all the user connected
void *createNewChannel(void *cmd);
void checkChannel(List *clients, int client, int freePlaces, char *message);
void joinChannel(char *msg, ChannelList *channelList, int client, List *clients);
void modifyChannel(ChannelList *channelList, char *message, int client, List *clients);
void removeChannel(char *msg, ChannelList *channelList, int client, List *clients);

#endif // CHANNEL_H_