#include <stdlib.h>
#include <error.h>
#include <ucontext.h>
#include "light_coroutine.h"

struct LcCoroutine {
	LcScheduler *scheduler;
	LcCoroutine *link;
	ucontext_t ucontext;
	LcFunction function;
	LcStatus status;
	size_t stack_size;
	void *returnValue;
};

struct LcScheduler {
	ucontext_t ucontext;
	LcCoroutine *current_coroutine;
};

static const char *lc_status_str(LcStatus status) {
	switch (status) {
	case LC_NEW:
		return "new";
	case LC_RUNNING:
		return "running";
	case LC_SUSPENDED:
		return "suspended";
	case LC_WAITING:
		return "waiting";
	case LC_DEAD:
		return "dead";
	default:
		return "unknown";
	}
}

LcScheduler *lc_scheduler_new() {
	return calloc(1, sizeof(LcScheduler));
}

void lc_scheduler_free(LcScheduler *scheduler) {
	free(scheduler);
}

LcCoroutine *lc_current(LcScheduler *scheduler) {
	return scheduler->current_coroutine;
}

LcCoroutine *lc_new(LcScheduler *scheduler, size_t stack_size,
		LcFunction function) {
	LcCoroutine *coroutine = calloc(1, sizeof(LcCoroutine));
	coroutine->scheduler = scheduler;
	coroutine->stack_size = stack_size;
	coroutine->function = function;
	coroutine->status = LC_NEW;
	return coroutine;
}

static void coroutine_function_wrapper(LcCoroutine *coroutine, void *argument) {
	coroutine->returnValue = coroutine->function(argument);
	coroutine->status = LC_DEAD;
}

void *lc_resume(LcCoroutine *coroutine, void *argument) {
	if (coroutine->status == LC_NEW) {
		coroutine->link = coroutine->scheduler->current_coroutine;
		getcontext(&(coroutine->ucontext));
		coroutine->ucontext.uc_stack.ss_sp = malloc(coroutine->stack_size);
		coroutine->ucontext.uc_stack.ss_size = coroutine->stack_size;
		coroutine->ucontext.uc_stack.ss_flags = 0;
		if (coroutine->link) {
			coroutine->ucontext.uc_link = &(coroutine->link->ucontext);
			coroutine->link->status = LC_WAITING;
		} else {
			coroutine->ucontext.uc_link = &(coroutine->scheduler->ucontext);
		}
		coroutine->status = LC_RUNNING;
		coroutine->scheduler->current_coroutine = coroutine;
		makecontext(&(coroutine->ucontext),
				(void (*)(void)) coroutine_function_wrapper, 2, coroutine,
				argument);
		swapcontext(coroutine->ucontext.uc_link, &(coroutine->ucontext));
		if (coroutine->link) {
			coroutine->link->status = LC_RUNNING;
			coroutine->scheduler->current_coroutine = coroutine->link;
		} else {
			coroutine->scheduler->current_coroutine = NULL;
		}
		return coroutine->returnValue;
	} else if (coroutine->status == LC_SUSPENDED) {
		if (coroutine->link != coroutine->scheduler->current_coroutine) {
			error(0, 0, "You can only resume your child coroutine.");
			return NULL;
		}
		if (coroutine->link) {
			coroutine->link->status = LC_WAITING;
		}
		coroutine->status = LC_RUNNING;
		coroutine->scheduler->current_coroutine = coroutine;
		coroutine->returnValue = argument;
		swapcontext(coroutine->ucontext.uc_link, &(coroutine->ucontext));
		if (coroutine->link) {
			coroutine->link->status = LC_RUNNING;
			coroutine->scheduler->current_coroutine = coroutine->link;
		} else {
			coroutine->scheduler->current_coroutine = NULL;
		}
		return coroutine->returnValue;
	} else {
		error(0, 0, "Can not resume %s coroutine.",
				lc_status_str(coroutine->status));
		return NULL;
	}
}

void *lc_yield(LcScheduler *scheduler, void *argument) {
	LcCoroutine *coroutine = scheduler->current_coroutine;
	if (coroutine == NULL) {
		error(0, 0,
				"Can not yield thread.");
		return NULL;
	}
	coroutine->returnValue = argument;
	coroutine->status = LC_SUSPENDED;
	if (coroutine->link) {
		coroutine->link->status = LC_RUNNING;
		scheduler->current_coroutine = coroutine->link;
	} else {
		scheduler->current_coroutine = NULL;
	}
	swapcontext(&(coroutine->ucontext), coroutine->ucontext.uc_link);
	coroutine->status = LC_RUNNING;
	if (coroutine->link) {
		coroutine->link->status = LC_WAITING;
	}
	scheduler->current_coroutine = coroutine;
	return coroutine->returnValue;
}

LcStatus lc_status(LcCoroutine *coroutine) {
	return coroutine->status;
}

bool lc_yieldable(LcScheduler *scheduler) {
	return scheduler->current_coroutine != NULL;
}

void lc_free(LcCoroutine *coroutine) {
	void *stack = coroutine->ucontext.uc_stack.ss_sp;
	if (stack) {
		free(stack);
	}
	free(coroutine);
}