#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/form.h"
#include "../headers/tools.h"
#include "../headers/commandServer.h"

Form *createForm(char *question, int maxAnswers)
{
    Form *newForm = (Form *)malloc(sizeof(Form));
    newForm->question = question;
    newForm->answerExpected = maxAnswers;
    newForm->answerNumber = 0;
    newForm->yes = 0;
    newForm->no = 0;
    newForm->next = NULL;
    return newForm;
}

void checkAnswer(char *message, int client, FormList *forms)
{
    int resRegex = regex(message, "^/aform +([0-9]{1}) +(Yes|yes|y|No|no|n) *$");
    if (resRegex != 0)
    {
        sendSpecificMessage(client, "\nYou can not answer this\n");
    }
    else
    {
        char *arr[3];
        getRegexGroup(arr, 3, message, "^/aform +([0-9]{1}) +(Yes|yes|Y|y|No|no|N|n) *$");
        if (strcmp(arr[2], "Yes") == 0 || strcmp(arr[2], "yes") == 0 || strcmp(arr[2], "Y") == 0 || strcmp(arr[2], "y") == 0)
        {
            answerForm(atoi(arr[1]), 1, forms);
        }
        else
        {
            answerForm(atoi(arr[1]), 0, forms);
        }
    }
}

void answerForm(int index, int answer, FormList *forms)
{
    Form *form = getFormByIndex(forms, index);
    if (form != NULL)
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
    }
}

void prepareSeeResults(int client, char *msg, FormList *list)
{
    int resRegex = regex(msg, "^/sform +([0-9]{1,2}) *$");
    if (resRegex == 0)
    {
        char *arr[2];
        getRegexGroup(arr, 2, msg, "^/sform +([0-9]{1,2}) *$");
        Form *form = getFormByIndex(list, atoi(arr[1]));
        sendSpecificMessage(client, seeResults(form));
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
