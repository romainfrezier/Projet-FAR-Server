/**
 * @file sema.c
 * @authors Romain FREZIER
 * @authors Etienne TILLIER
 * @brief Libraries for semaphore are not the same on Linux and Apple devices
 * @brief This library give the possibility to use functions on every devices
 * @version 0.1
 * @date 2022-05-26
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif
#include <stdio.h>
#include "../headers/sema.h"

void rk_sema_init(struct rk_sema *s, u_int32_t value)
{
#ifdef __APPLE__
    dispatch_semaphore_t *sem = &s->sem;

    *sem = dispatch_semaphore_create(value);
#else
    sem_init(&s->sem, 0, value);
#endif
}

void rk_sema_wait(struct rk_sema *s)
{
#ifdef __APPLE__
    dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
#else
    int r;
    do
    {
        r = sem_wait(&s->sem);
    } while (r == -1 && errno == EINTR);
#endif
}

void rk_sema_post(struct rk_sema *s)
{
#ifdef __APPLE__
    dispatch_semaphore_signal(s->sem);
#else
    sem_post(&s->sem);
#endif
}

void rk_sema_destroy(struct rk_sema *s)
{
#ifdef __APPLE__
    dispatch_release(s->sem);
#else
    sem_destroy(&s->sem);
#endif
}