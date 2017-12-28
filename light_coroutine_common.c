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
