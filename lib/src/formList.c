/**
 * @file formList.c
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Form list functions implementation
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/form.h"
#include "../headers/tools.h"
#include "../headers/commandServer.h"

FormList *createFormList(int size)
{
    FormList *newFormList = (FormList *)malloc(sizeof(FormList));
    newFormList->size = size;
    newFormList->head = NULL;
    return newFormList;
}

Form *getFormByIndex(FormList *forms, int index)
{
    int currentIndex = 1;
    Form *currentForm = forms->head;
    if (forms->size >= index)
    {
        while (currentForm != NULL && currentIndex < index)
        {
            currentForm = currentForm->next;
            currentIndex += 1;
        }
    }
    else
    {
        printf("The list cannot reach the current index ! \n");
    }
    return currentForm;
}

void addFormFirst(FormList *list, char *question, int maxAnwers, int client)
{
    if (list->size == 0)
    {
        sendSpecificMessage(client, "\nThere is no more space in the form-list !\n");
    }
    else
    {
        Form *newForm = createForm(question, maxAnwers);
        if (list->head == NULL)
        {
            list->head = newForm;
            newForm->next = NULL;
        }
        else
        {
            newForm->next = list->head;
            list->head = newForm;
        }
        list->size = list->size - 1;
        sendSpecificMessage(client, "\nThe form has been created !\n");
    }
}

char *listForm(FormList *list)
{
    Form *currentForm = list->head;
    char *formList = "\nList of forms in the channel : \n\n";
    char *finalString = (char *)malloc(strlen(formList));
    int currentIndex = 0;
    strcpy(finalString, formList);
    char *form = (char*)malloc(170);
    while (currentForm != NULL)
    {
        bzero(form, 170);
        currentIndex++;
        strcat(form, "\t");
        char indexString[10];
        sprintf(indexString, "%d", currentIndex);
        strcat(form, indexString);
        strcat(form, ". ");
        strcat(form, currentForm->question);
        strcat(form, " (");
        if ((currentForm->answerExpected - currentForm->answerNumber) < 0)
        {
            strcat(form, "0");
        }
        else
        {
            char expected[10];
            sprintf(expected, "%d", (currentForm->answerExpected - currentForm->answerNumber));
            strcat(form, expected);
        }
        strcat(form, " answer expected)");
        strcat(form, "\n");
        strcat(form, "\t\t• ");
        char numberOfYes[10];
        sprintf(numberOfYes, "%d", currentForm->yes);
        strcat(form, "yes : ");
        strcat(form, numberOfYes);
        strcat(form, " vote\n");
        strcat(form, "\t\t• ");
        char numberOfNo[10];
        sprintf(numberOfNo, "%d", currentForm->no);
        strcat(form, "no : ");
        strcat(form, numberOfNo);
        strcat(form, " vote\n");
        finalString = (char *)realloc(finalString, (strlen(finalString) + strlen(form)) * sizeof(char));
        strcat(finalString, form);
        currentForm = currentForm->next;
    }
    free(form);
    if (currentIndex == 0)
    {
        finalString = realloc(finalString, strlen("\nThe list is empty\n"));
        finalString = "\nThe list is empty\n";
    }
    
    return finalString;
}

void prepareRemoveByIndex(char *msg, FormList *list, int client, List *clients)
{
    if (isUserAdmin(clients, client) == 1)
    {
        if (regex(msg, "^/rmform +([0-9]{1,2}) *$") == 0)
        {
            char *arr[2];
            getRegexGroup(arr, 2, msg, "^/rmform +([0-9]{1,2}) *$");
            removeFormByIndex(list, atoi(arr[1]), client);
        }
        else
        {
            sendSpecificMessage(client, "\nThe correct command is /rmform id-of-form\n");
        }
    }
    else
    {
        sendSpecificMessage(client, "\n\033[0;31mYou are not an admin ! \n");
    }
}

void removeFormByIndex(FormList *list, int index, int client)
{
    Form *currentForm = list->head;
    int currentIndex = 1;
    if (currentForm != NULL && index == 1)
    {
        list->head = list->head->next;
        free(currentForm);
    }
    else 
    {
        while (currentForm != NULL && currentIndex != index - 1)
        {
            currentForm = currentForm->next;
            currentIndex += 1;
        }
        if (currentForm != NULL)
        {
            Form *deleted = currentForm->next;
            currentForm->next = deleted->next;
            free(deleted);
        }
    }
    if (currentForm == NULL)
    {
        sendSpecificMessage(client, "\nThe form does not exist\n");
    }
    else
    {
        sendSpecificMessage(client, "\nThe form has been removed\n");
    }
}

void prepareAddFormFirst(FormList *forms, char *message, int client, List *clients)
{
    if (isUserAdmin(clients, client) == 1)
    {
        if (regex(message, "^/cform +(.*[^ ]) +([0-9]{1,2}) *$") == 0)
        {
            char *arr[3];
            getRegexGroup(arr, 3, message, "^/cform +(.*[^ ]) +([0-9]{1,2}) *$");
            addFormFirst(forms, arr[1], atoi(arr[2]), client);
        }
        else
        {
            sendSpecificMessage(client, "\nThe command is /cform question number-of-answer-expected (number-of-answer-expected < 100)\n");
        }
    }
    else
    {
        sendSpecificMessage(client, "\n\033[0;31mYou are not an admin ! \n");
    }
}