// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module http of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
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

#include <pisk/defines.h>

#include "HttpTask.h"

namespace pisk
{
namespace services
{
namespace http
{
	class Worker
	{
	public:
		virtual ~Worker() {}


		virtual void init_service() noexcept = 0;

		virtual void deinit_service() noexcept = 0;


		virtual bool can_push_task() const noexcept = 0;

		virtual void push_task(HttpTaskPtr&& task) noexcept = 0;

		virtual bool pop_completed_task(HttpTaskPtr& task) noexcept = 0;


		virtual bool perform() noexcept = 0;
	};
	using WorkerPtr = std::unique_ptr<Worker>;
}
}
}

