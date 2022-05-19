#ifndef LIST_H_ /* Include guard */
#define LIST_H_

typedef struct Link Link;

struct Link {
    int value;
    char* pseudo;
    int admin;
    Link* next;
};

typedef struct List List;

struct List{
    Link* head;
    int size;
};

List *createList(int size);                         // create an empty list
int listeIsEmpty(List *list);                       // check if the list is empty. Return 0 if the list is empty
void addFirst(List *list, int value, char *pseudo); // add a user to the first position of the list
Link *next(Link *link);                             // go to the next Link of the list
void delFirst(List *list);                          // delete the first element of the list
void delVal(List *list, int val);                   // delete an element of the list passed in arguments
void delValAux(Link *link, int val);                // recursive function to delete an element of the list
void displayList(List *list);                       // display the list
int pseudoInList(List *list, char *pseudo);         // chek if the pseudo is in the list. Return 0 if the pseudo is in the list
int getIdByPseudo(List *list, char *pseudo);        // get the id from the pseudo
char *getPseudoById(List *list, int id);            // get the pseudo from the id
void setUserAdmin(List* list, int idClient);        // make a user as an admin
int isUserAdmin(List *list, int idClient);          // check if the user is an admin
char *getAllUsers(List *list, int client);          // get all the users of the list
void setPseudo(List *list, int client, char* pseudo);// get all the users of the list

#endif // LIST_H_