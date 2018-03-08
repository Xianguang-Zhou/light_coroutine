/*
 * Copyright (c) 2017, 2018, Xianguang Zhou <xianguang.zhou@outlook.com>. All rights reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIGHT_COROUTINE_H
#define LIGHT_COROUTINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef void *(*LcFunction)(void *argument);
typedef enum {
	LC_NEW, LC_RUNNING, LC_SUSPENDED, LC_WAITING, LC_DEAD
}LcStatus;
typedef struct LcCoroutine LcCoroutine;
typedef struct {
	void *argument;
	const char *error;
}LcArgument;

void lc_open();
void lc_close();
LcCoroutine *lc_current();
LcCoroutine *lc_new(size_t stack_size, LcFunction function);
LcArgument lc_resume(LcCoroutine *coroutine, void *argument);
LcArgument lc_yield(void *argument);
bool lc_resumable(LcCoroutine *coroutine);
bool lc_yieldable();
void lc_free(LcCoroutine *coroutine);
LcStatus lc_status(LcCoroutine *coroutine);
const char *lc_status_str(LcStatus status);
size_t lc_stack_size(LcCoroutine *coroutine);
void lc_set_data(LcCoroutine *coroutine, void *data);
void *lc_get_data(LcCoroutine *coroutine);

#ifdef __cplusplus
}
#endif

#endif
