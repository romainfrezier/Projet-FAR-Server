#ifndef CHANEL_H_ /* Include guard */
#define CHANEL_H_

#include <pthread.h>
#include "list.h"

typedef struct Chanel Chanel;

struct Chanel {
    int port;
    char* name;
    List* clients;
    Chanel* next;
    pthread_t thread;
};

typedef struct ChanelList ChanelList;

struct ChanelList
{
    Chanel *head;
    int size;
};

typedef struct ChanelStruct ChanelStruct;

struct ChanelStruct {
    ChanelList* chanelList;
    int port;
    char* name;
    pthread_t thread;
};

ChanelList *createChanelList(int size);
void addFirstChanel(ChanelList *list, Chanel *chanel);
int chanelListIsEmpty(ChanelList *list);
Chanel* createChanel(char* name, int port, pthread_t thread);

#endif // CHANEL_H_