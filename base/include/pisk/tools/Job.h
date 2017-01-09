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
#include "../utils/noncopyable.h"
#include "../infrastructure/Exception.h"
#include "../infrastructure/Logger.h"

#include <functional>
#include <atomic>
#include <thread>

namespace pisk
{
namespace tools
{
	class ScopedJob final :
		public utils::noncopyable
	{
	public:
		using JobRunFn = std::function<void(const ScopedJob&) noexcept>;

	private:
		std::atomic_bool stop_flag;
		JobRunFn run;
		std::thread thread;

		void safe_run()
		{
			infrastructure::Logger::get().debug("job", "Safe run: 0x%x", this);
			if (run != nullptr)
				run(*this);
			else
				infrastructure::Logger::get().warning("job", "Try run nullptr: 0x%x", this);
		}

	public:
		explicit ScopedJob(JobRunFn _run) :
			stop_flag(false),
			run(_run),
			thread(std::bind(&ScopedJob::safe_run, this))
		{
			infrastructure::Logger::get().debug("job", "Constructed: 0x%x", this);
			if (run == nullptr)
			{
				thread.join();
				throw infrastructure::InvalidArgumentException();
			}
		}

		~ScopedJob()
		{
			infrastructure::Logger::get().debug("job", "Destructing: 0x%x", this);
			request_stop();
			join();
			infrastructure::Logger::get().debug("job", "Destructed: 0x%x", this);
		}

		bool is_stopped() const
		{
			return stop_flag;
		}

		bool not_stopped() const
		{
			return not is_stopped();
		}

	private:
		void request_stop()
		{
			infrastructure::Logger::get().debug("job", "Stop requested: 0x%x", this);
			stop_flag = true;
		}

		void join()
		{
			infrastructure::Logger::get().debug("job", "Join: 0x%x", this);
			assert(std::this_thread::get_id() != thread.get_id());
			thread.join();
		}
	};

	class CyclicalScopedJob final :
		public utils::noncopyable
	{
	public:
		using JobIterationFn = std::function<void(const CyclicalScopedJob&) noexcept>;
		using JobSetupFn = std::function<void() noexcept>;

	private:
		JobSetupFn on_start;
		JobIterationFn iteration;
		JobSetupFn on_end;
		ScopedJob job;

		void run()
		{
			if (iteration == nullptr)
				return;

			if (on_start)
				on_start();

			do
			{
				iteration(*this);
			}
			while (not_stopped());

			if (on_end)
				on_end();
		}

	public:
		explicit CyclicalScopedJob(
			JobIterationFn _iteration,
			JobSetupFn _on_end = nullptr,
			JobSetupFn _on_start = nullptr
		) :
			on_start(_on_start),
			iteration(_iteration),
			on_end(_on_end),
			job(std::bind(&CyclicalScopedJob::run, this))
		{
			if (iteration == nullptr)
				throw infrastructure::InvalidArgumentException();
		}

		bool not_stopped() const
		{
			return job.not_stopped();
		}

		bool is_stopped() const
		{
			return job.is_stopped();
		}
	};
}
}

