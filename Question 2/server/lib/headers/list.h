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

List* createList(int size);
int listeIsEmpty(List* list);
void addFirst(List* list, int value, char* pseudo);
Link* next(Link* link);
void delFirst(List* list);
void delVal(List* list, int val);
void delValAux(Link* link, int val);
void displayList(List* list);
int pseudoInList(List* list, char* pseudo);
int getIdByPseudo(List* list, char* pseudo);
char* getPseudoById(List* list, int id);
void setUserAdmin(List* list, int idClient);
int isUserAdmin(List *list, int idClient);
char *getAllUsers(List *list, int client);