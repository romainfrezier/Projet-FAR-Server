/**
 * @file form.c
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Form functions implementation
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
#include "../headers/list.h"

Form *createForm(char *question, int maxAnswers)
{
    Form *newForm = (Form *)malloc(sizeof(Form));
    newForm->question = question;
    newForm->answerExpected = maxAnswers;
    newForm->answerNumber = 0;
    newForm->yes = 0;
    newForm->no = 0;
    newForm->clients = createList(maxAnswers);
    newForm->next = NULL;
    return newForm;
}

void prepareAnswer(char *message, int client, FormList *forms, List *clients)
{
    if (regex(message, "^/aform +([0-9]{1}) +(Yes|yes|y|Y|No|no|n|N) *$") == 0)
    {
        char *arr[3];
        getRegexGroup(arr, 3, message, "^/aform +([0-9]{1}) +(Yes|yes|Y|y|No|no|N|n) *$");
        Form *form = getFormByIndex(forms, atoi(arr[1]));
        if (form != NULL) {
            if (pseudoInList(form->clients, getPseudoById(clients, client)) == 1) {
                if (strcmp(arr[2], "Yes") == 0 || strcmp(arr[2], "yes") == 0 || strcmp(arr[2], "Y") == 0 ||
                    strcmp(arr[2], "y") == 0) {
                    answerForm(1, form, client, getPseudoById(clients, client));
                } else {
                    answerForm(0, form, client, getPseudoById(clients, client));
                }
            }
            else
            {
                sendSpecificMessage(client, "\nYou already answer the question\n");
            }
        }
        else
        {
            sendSpecificMessage(client, "\nThe form does not exist !\n");
        }
    }
    else
    {
        sendSpecificMessage(client, "\nYou can not answer this\n");
    }
}

void answerForm( int answer, Form *form , int client, char *pseudo)
{
    if (answer == 1)
    {
        form->yes += 1;
    }
    else
    {
        form->no += 1;
    }
    form->answerNumber += 1;
    addFirst(form->clients, client, pseudo);
}

void prepareSeeResults(int client, char *msg, FormList *list)
{
    if (regex(msg, "^/sform +([0-9]{1,2}) *$") == 0)
    {
        char *arr[2];
        getRegexGroup(arr, 2, msg, "^/sform +([0-9]{1,2}) *$");
        Form *form = getFormByIndex(list, atoi(arr[1]));
        sendSpecificMessage(client, seeResults(form));
    }
    else
    {
        sendSpecificMessage(client, "\nThe command is /sform number\n");
    }
}

char *seeResults(Form *form)
{
    char* finalString;
    if (form->answerNumber == 0)
    {
        finalString = "\nThere is no answer yet ! \n";
    }
    else
    {
        int yesP = (form->yes * 100) / form->answerNumber;
        int noP = (form->no * 100) / form->answerNumber;
        char *formResults = (char *)malloc(strlen(form->question) * sizeof(char));
        strcpy(formResults, form->question);
        int numberOfCaracYes = yesP / 2;
        int numberOfCaracNo = noP / 2;
        finalString = (char *)malloc((strlen(formResults) + strlen("\n\t\n\t\t\n\t\t No : Yes : a a") + numberOfCaracNo + numberOfCaracYes + 8) * sizeof(char));
        strcat(finalString, "\n\t");
        strcat(finalString, formResults);
        strcat(finalString, "\n\t\t");
        strcat(finalString, "Yes : ");
        for (int i = 0; i < numberOfCaracYes; i++)
        {
            strcat(finalString, "#");
        }
        strcat(finalString, " ");
        char yesPercent[10];
        sprintf(yesPercent, "%d", yesP);
        strcat(finalString, yesPercent);
        strcat(finalString, "%");
        strcat(finalString, "\n\t\t");
        strcat(finalString, "No  : ");
        for (int i = 0; i < numberOfCaracNo; i++)
        {
            strcat(finalString, "#");
        }
        strcat(finalString, " ");
        char noPercent[10];
        sprintf(noPercent, "%d", noP);
        strcat(finalString, noPercent);
        strcat(finalString, "%\n");
    }

    return finalString;
}