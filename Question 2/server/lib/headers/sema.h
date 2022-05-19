#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
typedef struct rk_sema rk_sema;
struct rk_sema
{
#ifdef __APPLE__
    dispatch_semaphore_t sem;
#else
    sem_t sem;
#endif
};

void rk_sema_init(struct rk_sema *s, u_int32_t value);  // initialize a semaphore 
void rk_sema_wait(struct rk_sema *s);                   // wait a semaphore
void rk_sema_post(struct rk_sema *s);                   // post a semaphore
void rk_sema_destroy(struct rk_sema *s);                // destroy a semephore