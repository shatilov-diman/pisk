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
			logger::debug("job", "Safe run: {}", this);
			if (run != nullptr)
				run(*this);
			else
				logger::warning("job", "Try run nullptr: {}", this);
		}

	public:
		explicit ScopedJob(JobRunFn _run) :
			stop_flag(false),
			run(_run),
			thread(std::bind(&ScopedJob::safe_run, this))
		{
			logger::debug("job", "Constructed: {}", this);
			if (run == nullptr)
			{
				thread.join();
				throw infrastructure::InvalidArgumentException();
			}
		}

		~ScopedJob()
		{
			logger::debug("job", "Destructing: {}", this);
			request_stop();
			join();
			logger::debug("job", "Destructed: {}", this);
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
			logger::debug("job", "Stop requested: {}", this);
			stop_flag = true;
		}

		void join()
		{
			logger::debug("job", "Join: {}", this);
			assert(std::this_thread::get_id() != thread.get_id());
			thread.join();
		}
	};

	class CyclicalScopedJob final :
		public utils::noncopyable
	{
	public:
		using JobIterationFn = std::function<void(const CyclicalScopedJob&) noexcept>;
		using JobSetupFn = std::function<bool() noexcept>;
		using JobFinishFn = std::function<void() noexcept>;

	private:
		JobSetupFn on_start;
		JobIterationFn iteration;
		JobFinishFn on_end;
		ScopedJob job;

		void run()
		{
			if (iteration == nullptr)
				return;

			if (on_start)
				if (on_start() == false)
				{
					logger::warning("job", "Failed start job: on_start returned false; {}", this);
					return;
				}

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
			JobFinishFn _on_end = nullptr,
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

