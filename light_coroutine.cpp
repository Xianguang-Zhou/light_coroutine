/*
 * Copyright (c) 2017, 2018, Xianguang Zhou <xianguang.zhou@outlook.com>. All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "light_coroutine.h"
#include "light_coroutine.hpp"

namespace lc {

class Scheduler {
public:
	boost::shared_ptr<Coroutine> current_coroutine;
	boost::any local_data;
};

static thread_local Scheduler *scheduler;

Exception::Exception(const char *error) :
		message(error) {
}

void *Coroutine::coroutine_function_wrapper(void *argument) {
	Coroutine *coroutine_ptr = scheduler->current_coroutine.get();
	coroutine_ptr->return_value = coroutine_ptr->function(
			coroutine_ptr->return_value);
	return nullptr;
}

Coroutine::Coroutine() {
}

Coroutine::~Coroutine() {
	lc_free((LcCoroutine*) c_ptr);
}

boost::any Coroutine::resume(const boost::any& argument) throw (Exception) {
	return_value = argument;
	boost::shared_ptr<Coroutine> current_coroutine =
			scheduler->current_coroutine;
	scheduler->current_coroutine = this->weak_ptr.lock();
	const char *error = lc_resume((LcCoroutine*) c_ptr, nullptr).error;
	scheduler->current_coroutine = current_coroutine;
	if (error != nullptr) {
		throw Exception(error);
	}
	return return_value;
}

bool Coroutine::resumable() const {
	return lc_resumable((LcCoroutine*) c_ptr);
}

Status Coroutine::status() const {
	LcStatus c_status = lc_status((LcCoroutine*) c_ptr);
	switch (c_status) {
	case LC_NEW:
		return Status::NEW;
	case LC_RUNNING:
		return Status::RUNNING;
	case LC_SUSPENDED:
		return Status::SUSPENDED;
	case LC_WAITING:
		return Status::WAITING;
	case LC_DEAD:
		return Status::DEAD;
	}
}

size_t Coroutine::stack_size() const {
	return lc_stack_size((LcCoroutine*) c_ptr);
}

void Coroutine::set_data(const boost::any& data) {
	local_data = data;
}

boost::any Coroutine::get_data() const {
	return local_data;
}

void open() {
	lc_open();
	scheduler = new Scheduler();
}

void close() {
	delete scheduler;
	lc_close();
}

boost::weak_ptr<Coroutine> current() {
	return boost::weak_ptr<Coroutine>(scheduler->current_coroutine);
}

boost::shared_ptr<Coroutine> create(const size_t& stack_size,
		const Function& function) {
	Coroutine *coroutine = new Coroutine();
	boost::shared_ptr<Coroutine> shared_ptr(coroutine);
	coroutine->weak_ptr = shared_ptr;
	coroutine->function = function;
	coroutine->c_ptr = lc_new(stack_size,
			Coroutine::coroutine_function_wrapper);
	return shared_ptr;
}

boost::any yield(const boost::any& argument) throw (Exception) {
	Coroutine *coroutine = scheduler->current_coroutine.get();
	if (coroutine == nullptr) {
		throw Exception(
				"You can not yield thread, you can only yield coroutine.");
	}
	coroutine->return_value = argument;
	const char *error = lc_yield(nullptr).error;
	if (error != nullptr) {
		throw Exception(error);
	}
	return coroutine->return_value;
}

bool yieldable() {
	return lc_yieldable();
}

std::string status_str(const Status& status) {
	switch (status) {
	case Status::NEW:
		return "new";
	case Status::RUNNING:
		return "running";
	case Status::SUSPENDED:
		return "suspended";
	case Status::WAITING:
		return "waiting";
	case Status::DEAD:
		return "dead";
	default:
		return "unknown";
	}
}

void set_data(const boost::any& data) {
	if (scheduler->current_coroutine != nullptr) {
		scheduler->current_coroutine->set_data(data);
	} else {
		scheduler->local_data = data;
	}
}

boost::any get_data() {
	if (scheduler->current_coroutine != nullptr) {
		return scheduler->current_coroutine->get_data();
	} else {
		return scheduler->local_data;
	}
}

}
