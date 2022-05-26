/**
 * @file sema.h
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

#ifndef SEMA_H_ /* Include guard */
#define SEMA_H_

/**
 * @brief use the good library
 */
typedef struct rk_sema rk_sema;

/**
 * @brief use the good library
 */
struct rk_sema
{
#ifdef __APPLE__
    dispatch_semaphore_t sem;
#else
    sem_t sem;
#endif
};

/**
 * @brief Initialize a semaphore
 *
 * @param s : the semaphore
 * @param value : the origin value of the semaphore
 */
void rk_sema_init(struct rk_sema *s, u_int32_t value);

/**
 * @brief Wait a semaphore
 *
 * @param s : the semaphore
 */
void rk_sema_wait(struct rk_sema *s);

/**
 * @brief Post a semaphore
 *
 * @param s : the semaphore
 */
void rk_sema_post(struct rk_sema *s);

/**
 * @brief Destroy a semaphore
 *
 * @param s : the semaphore
 */
void rk_sema_destroy(struct rk_sema *s);

#endif // SEMA_H_