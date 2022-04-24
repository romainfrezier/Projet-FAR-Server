#include "stringFunc.h"

char** str_split(char *msg)
{
    printf("Message : %s\n", msg);
    int i = 0;
    char* p = strtok(msg, " ");
    char** array =(char**)malloc(3*sizeof(char*));
    array[0] = (char*)malloc(strlen(p) + 1);
    strcpy(array[0], p);
    p = strtok(NULL, " ");
    array[1] = (char*)malloc(strlen(p) + 1);
    strcpy(array[1], p);
    p = strtok(NULL, "\0");
    array[2] = (char*)malloc(strlen(p) + 1);
    strcpy(array[2], p);
    return array;
}
