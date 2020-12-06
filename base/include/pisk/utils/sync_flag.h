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

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace pisk
{
namespace utils
{
namespace sync
{
	class flag
	{
		mutable std::mutex guard;
		mutable std::condition_variable signaler;
		std::atomic_bool signaled{false};

	public:
		bool get() const threadsafe
		{
			return signaled;
		}
		void set() threadsafe
		{
			if (signaled)
				return;
			std::unique_lock<std::mutex> lock(guard);
			signaled = true;
			signaler.notify_all();
		}
		void wait() const threadsafe
		{
			if (signaled)
				return;
			std::unique_lock<std::mutex> lock(guard);
			signaler.wait(lock, [this]() -> bool {return signaled;} );
		}
		template <typename Rep, typename Period>
		bool wait_for(const std::chrono::duration<Rep, Period>& timeout) const threadsafe
		{
			if (signaled)
				return true;
			std::unique_lock<std::mutex> lock(guard);
			return signaler.wait_for(lock, timeout, [this]() -> bool {return signaled;} );
		}
		template <typename Clock, typename Duration>
		bool wait_until(const std::chrono::time_point<Clock, Duration>& timepoint) const threadsafe
		{
			if (signaled)
				return true;
			std::unique_lock<std::mutex> lock(guard);
			return signaler.wait_until(lock, timepoint, [this]() -> bool {return signaled;} );
		}
	};
}//namespace sync
}//namespace utils
}//namespace pisk

