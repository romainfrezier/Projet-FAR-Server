#ifndef CHANEL_LIST_H_ /* Include guard */
#define CHANEL_LIST_H_

typedef struct ChanelList ChanelList;

struct ChanelList {
    Chanel* head;
    int size;
};

typedef struct Chanel Chanel;

struct Chanel {
    int port;
    char* name;
    List* clients;
    ChanelList* chanelList;
    Chanel* next;
};

Chanel* createChanel(char* name, int port);


#endif // CHANEL_LIST_H_