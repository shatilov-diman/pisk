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


		virtual bool init_service() noexcept = 0;

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

