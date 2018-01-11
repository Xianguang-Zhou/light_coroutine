/*
 * Copyright (c) 2017, 2018, Xianguang Zhou <xianguang.zhou@outlook.com>. All rights reserved.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "light_coroutine.h"

const char *lc_status_str(LcStatus status) {
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
