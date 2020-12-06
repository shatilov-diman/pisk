// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// Original sources:
//   https://github.com/shatilov-diman/pisk/
//   https://bitbucket.org/charivariltd/pisk/
//
// Author contacts:
//   Dmitry Shatilov (e-mail: shatilov.diman@gmail.com; site: https://www.linkedin.com/in/shatilov)
//
//


#pragma once

#include "../defines.h"
#include "noncopyable.h"

#include <memory>
#include <mutex>
#include <deque>
#include <tuple>

namespace pisk
{
namespace utils
{

template <typename Data>
class safequeue :
	public noncopyable,
	public nonmoveable
{
private:
	mutable std::mutex mutex;
	std::deque<Data> data_queue;

public:
	void push(Data&& data) threadsafe
	{
		std::unique_lock<std::mutex> guard(mutex);
		data_queue.emplace_back(std::move(data));
	}
	void push(const Data& data) threadsafe
	{
		std::unique_lock<std::mutex> guard(mutex);
		data_queue.emplace_back(data);
	}
	bool pop(Data& out) threadsafe
	{
		std::unique_lock<std::mutex> guard(mutex);
		if (data_queue.empty())
			return false;
		out = std::move(data_queue.front());
		data_queue.pop_front();
		return true;
	}
	bool empty() const threadsafe
	{
		std::unique_lock<std::mutex> guard(mutex);
		return data_queue.empty();
	}
	std::size_t size() const threadsafe
	{
		std::unique_lock<std::mutex> guard(mutex);
		return data_queue.size();
	}
};

}
}

