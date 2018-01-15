/*
 * Copyright (c) 2017, 2018, Xianguang Zhou <xianguang.zhou@outlook.com>. All rights reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef _WIN32

#include <stdlib.h>
#if defined(__APPLE__) && defined(__MACH__)
#define _XOPEN_SOURCE
#endif
#include <ucontext.h>
#include "light_coroutine.h"

struct LcCoroutine {
	LcScheduler *scheduler;
	LcCoroutine *link;
	ucontext_t ucontext;
	LcFunction function;
	LcStatus status;
	size_t stack_size;
	void *return_value;
};

struct LcScheduler {
	ucontext_t ucontext;
	LcCoroutine *current_coroutine;
};

LcScheduler *lc_scheduler_new() {
	return (LcScheduler *) calloc(1, sizeof(LcScheduler));
}

void lc_scheduler_free(LcScheduler *scheduler) {
	free(scheduler);
}

LcCoroutine *lc_current(LcScheduler *scheduler) {
	return scheduler->current_coroutine;
}

LcCoroutine *lc_new(LcScheduler *scheduler, size_t stack_size,
		LcFunction function) {
	LcCoroutine *coroutine = (LcCoroutine *) calloc(1, sizeof(LcCoroutine));
	coroutine->scheduler = scheduler;
	coroutine->stack_size = stack_size;
	coroutine->function = function;
	coroutine->status = LC_NEW;
	return coroutine;
}

static void coroutine_function_wrapper(LcCoroutine *coroutine) {
	coroutine->return_value = coroutine->function(coroutine->return_value);
	coroutine->status = LC_DEAD;
}

LcArgument lc_resume(LcCoroutine *coroutine, void *argument) {
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
		coroutine->return_value = argument;
		makecontext(&(coroutine->ucontext),
				(void (*)(void)) coroutine_function_wrapper, 1, coroutine);
		swapcontext(coroutine->ucontext.uc_link, &(coroutine->ucontext));
		if (coroutine->link) {
			coroutine->link->status = LC_RUNNING;
			coroutine->scheduler->current_coroutine = coroutine->link;
		} else {
			coroutine->scheduler->current_coroutine = NULL;
		}
		LcArgument return_argument = { .argument = coroutine->return_value,
				.error = NULL };
		return return_argument;
	} else if (coroutine->status == LC_SUSPENDED) {
		if (coroutine->link != coroutine->scheduler->current_coroutine) {
			LcArgument return_argument = { .argument = NULL, .error =
					"You can only resume your child coroutine." };
			return return_argument;
		}
		if (coroutine->link) {
			coroutine->link->status = LC_WAITING;
		}
		coroutine->status = LC_RUNNING;
		coroutine->scheduler->current_coroutine = coroutine;
		coroutine->return_value = argument;
		swapcontext(coroutine->ucontext.uc_link, &(coroutine->ucontext));
		if (coroutine->link) {
			coroutine->link->status = LC_RUNNING;
			coroutine->scheduler->current_coroutine = coroutine->link;
		} else {
			coroutine->scheduler->current_coroutine = NULL;
		}
		LcArgument return_argument = { .argument = coroutine->return_value,
				.error = NULL };
		return return_argument;
	} else {
		LcArgument return_argument = { .argument = NULL, .error =
				"You can only resume new or suspended coroutine." };
		return return_argument;
	}
}

LcArgument lc_yield(LcScheduler *scheduler, void *argument) {
	LcCoroutine *coroutine = scheduler->current_coroutine;
	if (coroutine == NULL) {
		LcArgument return_argument = { .argument = NULL, .error =
				"You can not yield thread, you can only yield coroutine." };
		return return_argument;
	}
	coroutine->return_value = argument;
	coroutine->status = LC_SUSPENDED;
	swapcontext(&(coroutine->ucontext), coroutine->ucontext.uc_link);
	LcArgument return_argument = { .argument = coroutine->return_value, .error =
	NULL };
	return return_argument;
}

LcStatus lc_status(LcCoroutine *coroutine) {
	return coroutine->status;
}

bool lc_resumable(LcCoroutine *coroutine) {
	if (coroutine->status == LC_NEW) {
		return true;
	} else if (coroutine->status == LC_SUSPENDED) {
		return coroutine->link == coroutine->scheduler->current_coroutine;
	} else {
		return false;
	}
}

bool lc_yieldable(LcScheduler *scheduler) {
	return scheduler->current_coroutine != NULL ;
}

void lc_free(LcCoroutine *coroutine) {
	void *stack = coroutine->ucontext.uc_stack.ss_sp;
	if (stack) {
		free(stack);
	}
	free(coroutine);
}

size_t lc_stack_size(LcCoroutine *coroutine) {
	return coroutine->stack_size;
}

#endif
