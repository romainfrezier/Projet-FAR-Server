#include <stdio.h>
#include "colors.h"

// Functions to print in color
// Green for transmitting message
void green()
{
    printf("\033[0;32m");
}
// Blue for receiving message
void blue()
{
    printf("\033[0;34m");
}
// Purple for receiving private message
void purple()
{
    printf("\033[0;35m");
}
// Red for errors
void red()
{
    printf("\033[0;31m");
}
// Reset color to white
void reset()
{
    printf("\033[0m");
}