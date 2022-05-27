/**
 * @file main.c
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Main function of server program
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "lib/headers/server.h"

/**
 * @brief Main function of server program
 * @param argc number of arguments given
 * @param argv arguments given
 * @return 0
 */
int main(int argc, char *argv[])
{
    launchServer();
    return 0;
}