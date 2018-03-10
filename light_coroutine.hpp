/*
 * Copyright (c) 2017, 2018, Xianguang Zhou <xianguang.zhou@outlook.com>. All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIGHT_COROUTINE_HPP
#define LIGHT_COROUTINE_HPP

#include <functional>
#include <boost/any.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace lc {

typedef std::function<boost::any(boost::any)> Function;

enum class Status {
	NEW, RUNNING, SUSPENDED, WAITING, DEAD
};

class Exception {
public:
	explicit Exception(const char *error);
	const std::string message;
};

class Coroutine: private boost::noncopyable {
public:
	~Coroutine();
	boost::any resume(const boost::any& argument) throw (Exception);
	bool resumable() const;
	Status status() const;
	size_t stack_size() const;
	void set_data(const boost::any& data);
	boost::any get_data() const;
	inline void data(const boost::any& data) {
		set_data(data);
	}
	inline boost::any data() const {
		return get_data();
	}
private:
	void *c_ptr;
	boost::weak_ptr<Coroutine> weak_ptr;
	Function function;
	boost::any return_value;
	boost::any local_data;
	static void *coroutine_function_wrapper(void *argument);
	explicit Coroutine();

	friend boost::shared_ptr<Coroutine> create(const size_t& stack_size,
			const Function& function);
	friend boost::any yield(const boost::any& argument) throw (Exception);
};

void open();
void close();
boost::weak_ptr<Coroutine> current();
boost::shared_ptr<Coroutine> create(const size_t& stack_size,
		const Function& function);
boost::any yield(const boost::any& argument) throw (Exception);
bool yieldable();
std::string status_str(const Status& status);
void set_data(const boost::any& data);
boost::any get_data();
inline void data(const boost::any& data) {
	set_data(data);
}
inline boost::any data() {
	return get_data();
}

}
#endif /* LIGHT_COROUTINE_HPP */
