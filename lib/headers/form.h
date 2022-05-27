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

typedef struct Form Form;

struct Form
{
    char *question;
    int answerNumber;
    int answerExpected;
    int yes;
    int no;
    Form *next;
};

typedef struct FormList FormList;

struct FormList
{
    Form *head;
    int size;
};

Form *createForm(char *question, int maxAnswers); //

void checkAnswer(char *message, int client, FormList *forms); //

void answerForm(int index, int answer, FormList *forms); //

Form *getFormByIndex(FormList *list, int index); //

FormList *createFormList(int size); //

void addFormFirst(FormList *list, char *question, int maxAnwers, int client); //

char *listForm(FormList *list); //

void prepareRemoveByIndex(char *msg, FormList *list, int client, List *clients);

void removeFormByIndex(FormList *list, int index, int client); //

void prepareAddFormFirst(FormList *forms, char *message, int client, List *clients);

void prepareSeeResults(int client, char *msg, FormList* list);

char *seeResults(Form *form);

char *formAnswer(Form *form);

#endif // FORM_H_