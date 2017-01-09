// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional restriction according to GPLv3 pt 7:
// b) required preservation author attributions;
// c) required preservation links to original sources
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

