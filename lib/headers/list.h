/**
 * @file list.h
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief list of clients functions
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef LIST_H_ /* Include guard */
#define LIST_H_

/**
 * @brief Client data
 */
typedef struct Client Client;

/**
 * @brief Client data
 *
 * @param id id
 * @param pseudo pseudo of the client
 * @param admin -1 not admin; 1 admin
 * @param alreadyConnected 0 : connected; 1 : information for join channel
 * @param next next Client in the list
 */
struct Client {
    int id;
    char* pseudo;
    int admin;
    int alreadyConnected;
    Client *next;
};

/**
 * @brief Clients list data
 */
typedef struct List List;

/**
 * @brief Clients list data
 *
 * @param head head of the linked list
 * @param size free places in the list
 */
struct List{
    Client* head;
    int size;
};

/**
 * @brief Create an empty list
 *
 * @param size size of the list
 * @return the new List created
 */
List *createList(int size);

/**
 * @brief Check if the list is empty
 *
 * @param list list to check
 * @return 0 -> list is empty
 * @return 1 -> list is not empty
 */
int listIsEmpty(List *list);

/**
 * @brief Add a user to the first position of the list
 *
 * @param list list where the user will be added
 * @param id id of the client
 * @param pseudo pseudo of the client
 */
void addFirst(List *list, int id, char *pseudo);

/**
 * @brief Add a client to the first position of the list
 *
 * @param list list where the client will be added
 * @param client client data
 * @param pseudo pseudo of the client
 */
void addFirstClient(List *list, Client *client, char *pseudo);

/**
 * @brief Next client in the linked list
 *
 * @param client actual client
 * @return next client
 */
Client *next(Client *client);

/**
 * @brief Delete the first element of the list
 *
 * @param list list where the element will be deleted
 */
void delFirst(List *list);

/**
 * @brief Delete an element of the list passed in arguments
 *
 * @param list list where the element will be deleted
 * @param id id of the element to delete
 */
void delVal(List *list, int id);

/**
 * @brief Recursive function to delete an element of the list
 *
 * @param client client from where we start to search
 * @param id id of the element to delete
 */
void delValAux(Client *client, int id);

/**
 * @brief Display the list
 *
 * @param list list to display
 */
void displayList(List *list);

/**
 * @brief Check if the pseudo is in the list
 *
 * @param list list where the pseudo is searched
 * @param pseudo pseudo to search
 * @return 0 -> pseudo is in the list
 * @return 1 -> pseudo is not in the list
 */
int pseudoInList(List *list, char *pseudo);

/**
 * @brief Get the id from the pseudo
 *
 * @param list list where the id is searched
 * @param pseudo pseudo that correspond to the id
 * @return the id that correspond to the pseudo
 * @return -1 -> Error (pseudo not in the list)
 */
int getIdByPseudo(List *list, char *pseudo);

/**
 * @brief Get the pseudo from the id
 *
 * @param list list where the pseudo is searched
 * @param id id that correspond to the pseudo
 * @return the pseudo that correspond to the id
 * @return NULL -> Error (pseudo not in the list)
 */
char *getPseudoById(List *list, int id);

/**
 * @brief Make a user as an admin
 *
 * @param list list of clients where the client is
 * @param idClient id of the client
 */
void setUserAdmin(List* list, int idClient);

/**
 * @brief Check if the user is an admin
 *
 * @param list list of clients where the client is
 * @param idClient id of the client
 * @return 1 -> user is admin
 * @return -1 -> user is not admin
 */
int isUserAdmin(List *list, int idClient);

/**
 * @brief Get all the users of the list
 *
 * @param list list of clients in the current channel
 * @param idClient id of client who made the request
 * @param start Start of the message
 * @return the string of the whole list
 */
char *getAllChannelUsers(List *list, int idClient, char *start);

/**
 * @brief Set the pseudo of a user as "pseudo"
 * @param list list of clients where the client is
 * @param idClient id of client who made the request
 * @param pseudo new pseudo for the client
 */
void setPseudo(List *list, int idClient, char* pseudo);

/**
 * @brief Get whole client data with an id
 *
 * @param list list of clients
 * @param id id searched
 * @return Client data -> the client exists
 * @return NULL -> the client doesn't exist
 */
Client *getClientById(List *list, int id);

/**
 * @brief Change the status of the client when he wants to change of channel
 *
 * @param list list of clients where the client is
 * @param idClient id of the client
 */
void changeACforJoin(List *list, int idClient);

#endif // LIST_H_