/*
 * Copyright (c) 2017, 2018, Xianguang Zhou <xianguang.zhou@outlook.com>. All rights reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef _WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 // Windows Vista or Windows Server 2008
#endif

#include <stdlib.h>
#include <windows.h>

#if _WIN32_WINNT >= 0x0600 // Windows Vista or Windows Server 2008
#include <fibersapi.h>
#endif

#include "light_coroutine.h"

struct LcCoroutine {
	LcCoroutine *link;
	LPVOID fiber;
	LcFunction function;
	LcStatus status;
	size_t stack_size;
	void *return_value;
	void *local_data;
};

typedef struct {
	LPVOID fiber;
	LcCoroutine *current_coroutine;
	void *local_data;
} LcScheduler;

#ifdef _MSC_VER
static __declspec(thread) LcScheduler *scheduler;
#else
static __thread LcScheduler *scheduler;
#endif

void lc_open() {
#if _WIN32_WINNT >= 0x0600 // Windows Vista or Windows Server 2008
	if (!IsThreadAFiber()) {
#endif
		scheduler = (LcScheduler *)malloc(sizeof(LcScheduler));
#if _WIN32_WINNT >= 0x0502 // Windows Server 2003
		scheduler->fiber = ConvertThreadToFiberEx(scheduler, FIBER_FLAG_FLOAT_SWITCH);
#else // Windows XP
		scheduler->fiber = ConvertThreadToFiber(scheduler);
#endif
		scheduler->current_coroutine = NULL;
#if _WIN32_WINNT >= 0x0600 // Windows Vista or Windows Server 2008
	}
#endif
}

void lc_close() {
	if (scheduler->current_coroutine == NULL
#if _WIN32_WINNT >= 0x0600 // Windows Vista or Windows Server 2008
			&& IsThreadAFiber()
#endif
#ifdef GetCurrentFiber
			&& GetCurrentFiber() == scheduler->fiber
#endif
	) {
		ConvertFiberToThread();
		free(scheduler);
	}
}

LcCoroutine *lc_current() {
	return scheduler->current_coroutine;
}

LcCoroutine *lc_new(size_t stack_size, LcFunction function) {
	LcCoroutine *coroutine = (LcCoroutine *) calloc(1, sizeof(LcCoroutine));
	coroutine->stack_size = stack_size;
	coroutine->function = function;
	coroutine->status = LC_NEW;
	return coroutine;
}

static void coroutine_function_wrapper(LcCoroutine *coroutine) {
	coroutine->return_value = coroutine->function(coroutine->return_value);
	coroutine->status = LC_DEAD;
	if (coroutine->link) {
		SwitchToFiber(coroutine->link->fiber);
	} else {
		SwitchToFiber(scheduler->fiber);
	}
}

LcArgument lc_resume(LcCoroutine *coroutine, void *argument) {
	if (coroutine->status == LC_NEW) {
		coroutine->link = scheduler->current_coroutine;
		if (coroutine->link) {
			coroutine->link->status = LC_WAITING;
		}
		coroutine->status = LC_RUNNING;
		scheduler->current_coroutine = coroutine;
#if _WIN32_WINNT >= 0x0502 // Windows Server 2003
		coroutine->fiber = CreateFiberEx(
				coroutine->stack_size - 1,
				coroutine->stack_size,
				FIBER_FLAG_FLOAT_SWITCH,
				(LPFIBER_START_ROUTINE)coroutine_function_wrapper,
				coroutine);
#else // Windows XP
		coroutine->fiber = CreateFiberEx(
				coroutine->stack_size - 1,
				coroutine->stack_size,
				0,
				(LPFIBER_START_ROUTINE)coroutine_function_wrapper,
				coroutine);
#endif
		coroutine->return_value = argument;
		SwitchToFiber(coroutine->fiber);
		if (coroutine->link) {
			coroutine->link->status = LC_RUNNING;
			scheduler->current_coroutine = coroutine->link;
		} else {
			scheduler->current_coroutine = NULL;
		}
		LcArgument return_argument;
		return_argument.argument = coroutine->return_value,
		return_argument.error = NULL;
		return return_argument;
	} else if (coroutine->status == LC_SUSPENDED) {
		if (coroutine->link != scheduler->current_coroutine) {
			LcArgument return_argument;
			return_argument.argument = NULL;
			return_argument.error = "You can only resume your child coroutine.";
			return return_argument;
		}
		if (coroutine->link) {
			coroutine->link->status = LC_WAITING;
		}
		coroutine->status = LC_RUNNING;
		scheduler->current_coroutine = coroutine;
		coroutine->return_value = argument;
		SwitchToFiber(coroutine->fiber);
		if (coroutine->link) {
			coroutine->link->status = LC_RUNNING;
			scheduler->current_coroutine = coroutine->link;
		} else {
			scheduler->current_coroutine = NULL;
		}
		LcArgument return_argument;
		return_argument.argument = coroutine->return_value;
		return_argument.error = NULL;
		return return_argument;
	} else {
		LcArgument return_argument;
		return_argument.argument = NULL;
		return_argument.error = "You can only resume new or suspended coroutine.";
		return return_argument;
	}
}

LcArgument lc_yield(void *argument) {
	LcCoroutine *coroutine = scheduler->current_coroutine;
	if (coroutine == NULL) {
		LcArgument return_argument;
		return_argument.argument = NULL;
		return_argument.error = "You can not yield thread, you can only yield coroutine.";
		return return_argument;
	}
	coroutine->return_value = argument;
	coroutine->status = LC_SUSPENDED;
	if (coroutine->link) {
		SwitchToFiber(coroutine->link->fiber);
	} else {
		SwitchToFiber(scheduler->fiber);
	}
	LcArgument return_argument;
	return_argument.argument = coroutine->return_value;
	return_argument.error = NULL;
	return return_argument;
}

bool lc_resumable(LcCoroutine *coroutine) {
	if (coroutine->status == LC_NEW) {
		return true;
	} else if (coroutine->status == LC_SUSPENDED) {
		return coroutine->link == scheduler->current_coroutine;
	} else {
		return false;
	}
}

bool lc_yieldable() {
	return scheduler->current_coroutine != NULL;
}

void lc_free(LcCoroutine *coroutine) {
	if (coroutine->fiber) {
		DeleteFiber(coroutine->fiber);
	}
	free(coroutine);
}

LcStatus lc_status(LcCoroutine *coroutine) {
	return coroutine->status;
}

size_t lc_stack_size(LcCoroutine *coroutine) {
	return coroutine->stack_size;
}

void lc_set_data(LcCoroutine *coroutine, void *data) {
	if (coroutine != NULL) {
		coroutine->local_data = data;
	} else {
		scheduler->local_data = data;
	}
}

void *lc_get_data(LcCoroutine *coroutine) {
	if (coroutine != NULL) {
		return coroutine->local_data;
	} else {
		return scheduler->local_data;
	}
}

#endif
