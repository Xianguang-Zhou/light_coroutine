#ifndef LIGHT_COROUTINE_H
#define LIGHT_COROUTINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef void *(*LcFunction)(void *argument);
typedef enum {
	LC_NEW, LC_RUNNING, LC_SUSPENDED, LC_WAITING, LC_DEAD
} LcStatus;
typedef struct LcCoroutine LcCoroutine;
typedef struct LcScheduler LcScheduler;

LcScheduler *lc_scheduler_new();
void lc_scheduler_free(LcScheduler *scheduler);
LcCoroutine *lc_current(LcScheduler *scheduler);
LcCoroutine *lc_new(LcScheduler *scheduler, size_t stack_size,
		LcFunction function);
void *lc_resume(LcCoroutine *coroutine, void *argument);
void *lc_yield(LcScheduler *scheduler, void *argument);
LcStatus lc_status(LcCoroutine *coroutine);
bool lc_yieldable(LcScheduler *scheduler);
void lc_free(LcCoroutine *coroutine);

#ifdef __cplusplus
}
#endif

#endif
