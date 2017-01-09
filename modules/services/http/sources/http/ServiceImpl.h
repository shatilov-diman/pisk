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

#include <pisk/utils/safequeue.h>
#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/Job.h>

#include <pisk/http/Service.h>

#include <chrono>
#include <thread>

#include "Worker.h"

namespace pisk
{
namespace services
{
namespace http
{
	class ServiceImpl :
		public Service
	{
		utils::safequeue<HttpTaskPtr> requests;
		WorkerPtr worker;
		tools::CyclicalScopedJob job;

	public:
		explicit ServiceImpl(WorkerPtr&& worker):
			worker(std::move(worker)),
			job(
				std::bind(&ServiceImpl::iteration, this, std::placeholders::_1),
				std::bind(&ServiceImpl::deinit_service, this),
				std::bind(&ServiceImpl::init_service, this)
			)
		{}

	private:
		void init_service()
		{
			worker->init_service();
		}
		void deinit_service()
		{
			worker->deinit_service();
		}
		void iteration(const tools::CyclicalScopedJob&)
		{
			push_tasks();
			const bool all_tasks_completed = not worker->perform();
			if (all_tasks_completed)
				waiting_for_task();
			pop_completed_tasks();
		}

	private:
		void push_tasks()
		{
			while (worker->can_push_task())
			{
				HttpTaskPtr task;
				const bool empty = not requests.pop(task);
				if (empty)
					break;
				worker->push_task(std::move(task));
			}
		}

		void pop_completed_tasks()
		{
			while (true)
			{
				HttpTaskPtr task;
				const bool no_completed = not worker->pop_completed_task(task);
				if (no_completed)
					break;
				infrastructure::Logger::get().debug("http", "Response for url: %s", to_string(task->request.url).c_str());
				task->response.request = std::move(task->request);
				task->response_promise.set_value(std::move(task->response));
			}
		}

		void waiting_for_task() const
		{
			while (requests.empty() and not job.is_stopped())
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}


	private:
		virtual void release() final override
		{
			delete this;
		}

		virtual std::future<Response> request(const Request& request) noexcept threadsafe final override
		{
			infrastructure::Logger::get().debug("http", "Request by url: %s", to_string(request.url).c_str());
			auto task = std::make_unique<HttpTask>(HttpTask {request});
			auto future = task->response_promise.get_future();
			requests.push(std::move(task));
			return future;
		}
	};
}
}
}

