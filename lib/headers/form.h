/**
 * @file form.h
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Form functions
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef FORM_H_ /* Include guard */
#define FORM_H_

#include "list.h"

/**
 * @brief Data about a form
 */
typedef struct Form Form;

/**
 * @brief Data about a form
 *
 * @param question The question of the form
 * @param answerNumber Current number of answer
 * @param answerExpected Number of answer expected
 * @param yes Number of answer yes
 * @param no Number of answer no
 * @param clients List of clients who have already answer
 * @param next Next form in the linked list
 */
struct Form
{
    char *question;
    int answerNumber;
    int answerExpected;
    int yes;
    int no;
    List *clients;
    Form *next;
};

/**
 * @brief Linked list of forms
 */
typedef struct FormList FormList;

/**
 * @brief Linked list of forms
 *
 * @param head First element of the linked list
 * @param size Current size of the linked list
 */
struct FormList
{
    Form *head;
    int size;
};

/**
 * @brief Initialize a new form
 *
 * @param question The question of the form
 * @param maxAnswers Number of answer expected
 * @return The new form
 */
Form *createForm(char *question, int maxAnswers);

/**
 * @brief Check the message from the client and prepare the answer
 *
 * @param message Message from the client
 * @param client Id of the client who send the message
 * @param forms List of all the forms of the channel
 * @param clients List of clients in the current channel
 */
void prepareAnswer(char *message, int client, FormList *forms, List *clients);

/**
 * @brief Make an answer to a form
 *
 * @param answer Answer of the client
 * @param form Form to answer
 * @param client Id of the client who send the message
 * @param pseudo Pseudo of the client who send the message
 */
void answerForm(int answer, Form *form, int client, char *pseudo);

/**
 * @brief Get all the data about a form by index
 *
 * @param list List of all the forms of the channel
 * @param index Index of the form
 * @return The form that correspond to the index
 * @return NULL -> the form does not exist
 */
Form *getFormByIndex(FormList *list, int index);

/**
 * @brief Create an empty linked list of forms
 *
 * @param size size of the list
 * @return The new form list created
 */
FormList *createFormList(int size);

/**
 * @brief Add a form to the list
 *
 * @param list List of all the forms of the channel
 * @param question The question of the answer
 * @param maxAnwers Number of answer expected
 * @param client Id of the client who made the request
 */
void addFormFirst(FormList *list, char *question, int maxAnwers, int client);

/**
 * @brief Make a string of all the form of the channel
 *
 * @param list The list that will be treated
 * @return The string of the form list
 */
char *listForm(FormList *list);

/**
 * @brief Check the message from the client and prepare to remove a form
 *
 * @param msg Message from the client
 * @param list List of all the forms of the channel
 * @param client Id of the client who made the request
 * @param clients List of clients in the current channel
 */
void prepareRemoveByIndex(char *msg, FormList *list, int client, List *clients);

/**
 * @brief Remove a form by index
 *
 * @param list List of all the forms of the channel
 * @param index Index of the form that will be removed
 * @param client Id of the client who made the request
 */
void removeFormByIndex(FormList *list, int index, int client);

/**
 * @brief Check the message from the client and prepare to add a form to the list
 *
 * @param forms List of all the forms of the channel
 * @param message Message from the client
 * @param client Id of the client who made the request
 * @param clients List of clients in the current channel
 */
void prepareAddFormFirst(FormList *forms, char *message, int client, List *clients);

/**
 * @brief Check the message from the client and prepare to send the result of the form
 *
 * @param client Id of the client who made the request
 * @param msg Message from the client
 * @param list List of all the forms of the channel
 */
void prepareSeeResults(int client, char *msg, FormList* list);

/**
 * @brief Make a string of form results
 *
 * @param form The form that will be treated
 * @return The string of the form results
 */
char *seeResults(Form *form);

/**
 * @brief Check if a client has already answer to a form
 *
 * @param clients List of clients of the channel
 * @param client Client who try to answer
 * @param clientsWhoAnswer List of clients who have already answer
 * @return 0 -> Client has not already answer
 * @return 1 -> Client has already answer
 */
int clientAlreadyAnswer(List *clients, int client, List *clientsWhoAnswer);

#endif // FORM_H_