typedef struct Link Link;

struct Link {
    int value;
    Link* next;
};

typedef struct List List;

struct List{
    Link* head;
};

List* createList();

int listeIsEmpty(List* list);

void addFirst(List* list, int value);

Link* next(Link* link);

void delFirst(List* list);

void delVal(List* list, int val);

void delValAux(Link* link, int val);

