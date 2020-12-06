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
#include "../utils/safequeue.h"
#include "../infrastructure/Logger.h"
#include "../infrastructure/Exception.h"

#include <future>
#include <functional>
#include <type_traits>

namespace pisk
{
namespace tools
{
namespace _details
{
	class RemoteTask :
		public utils::noncopyable
	{
	public:
		using callee_t = void ();
		using result_t = void;

	private:
		std::function<callee_t> callee;
		std::promise<result_t> promise;

	public:
		RemoteTask() = default;
		RemoteTask(RemoteTask&&) = default;
		RemoteTask& operator=(RemoteTask&&) = default;

		RemoteTask(std::function<callee_t> callee) :
			callee(std::move(callee))
		{}

		bool valid() const
		{
			return callee != nullptr;
		}

		void swap(RemoteTask& out)
		{
			promise.swap(out.promise);
			callee.swap(out.callee);
		}

		void reset()
		{
			decltype(promise) trash;
			promise.swap(trash);
			callee = nullptr;
		}

		std::future<result_t> get_future()
		{
			return promise.get_future();
		}

		void operator ()()
		{
			callee();
			promise.set_value();
		}
	};

	class RemoteTaskList :
		public utils::noncopyable
	{
		utils::safequeue<RemoteTask> tasks;

	public:

		std::future<void> push(std::function<void()> handle) threadsafe
		{
			auto task = RemoteTask(handle);
			auto future = task.get_future();
			tasks.push(std::move(task));
			return future;
		}

		bool pop(RemoteTask& task) threadsafe
		{
			return tasks.pop(task);
		}
	};

}//namespace _details

	class RemoteTaskExecutor :
		public utils::noncopyable
	{
		_details::RemoteTaskList task_list;
		std::atomic<std::thread::id> execute_thread_id{std::thread::id{}};

	public:

		void init()
		{
			if (execute_thread_id != std::thread::id{})
			{
				pisk::logger::critical("RemoteTaskExecutor", "already initialized");
				throw infrastructure::InitializeError();
			}
			execute_thread_id = std::this_thread::get_id();
		}

		void deinit()
		{
			if (execute_thread_id != std::this_thread::get_id())
			{
				pisk::logger::error("RemoteTaskExecutor", "unable to execute 'deinit': RemoteTaskExecutor still no initialized");
				throw infrastructure::InitializeError();
			}

			execute_thread_id = std::thread::id{};
		}

		void execute_remote_tasks() threadsafe
		{
			check_initialized(__FUNCTION__);

			_details::RemoteTask task;
			while (task_list.pop(task))
				if (task.valid())
					task();
		}

		template <typename Callee>
		auto push_remote_task_sync(Callee&& runnable) threadsafe -> std::enable_if_t<!std::is_void<decltype(runnable())>::value, decltype(runnable())>
		{
			check_initialized(__FUNCTION__);

			if (execute_thread_id == std::this_thread::get_id())
				return runnable();

			decltype(runnable()) out;
			task_list.push([&runnable, &out]() {
				out = runnable();
			}).wait();
			return out;
		}
		template <typename Callee>
		auto push_remote_task_sync(Callee&& runnable) threadsafe -> std::enable_if_t<std::is_void<decltype(runnable())>::value>
		{
			check_initialized(__FUNCTION__);

			if (execute_thread_id == std::this_thread::get_id())
				return runnable();

			task_list.push([&runnable]() {
				runnable();
			}).wait();
		}

		template <typename Callee>
		auto push_remote_task_async(Callee&& runnable) threadsafe -> std::future<decltype(runnable())>
		{
			check_initialized(__FUNCTION__);

			struct data {
				std::promise<decltype(runnable())> _promise;
				std::remove_reference_t<Callee> _runnable;
			};
			auto d = std::make_shared<data>(data{{}, std::forward<Callee>(runnable)});
			auto out = d->_promise.get_future();
			task_list.push([d]() mutable {
				execute<decltype(runnable())>(std::move(d->_promise), std::move(d->_runnable));
			});
			return out;
		}

	private:
		template <std::size_t size>
		void check_initialized(const char (&fn_name)[size]) const threadsafe
		{
			if (execute_thread_id == std::thread::id{})
			{
				pisk::logger::error("RemoteTaskExecutor", "unable to execute '{}': RemoteTaskExecutor still no initialized", fn_name);
				throw infrastructure::InitializeError();
			}
		}

		template <typename Result, typename Callee, typename std::enable_if_t<!std::is_void<Result>::value, int> = 0>
		static void execute(std::promise<Result>&& promise, Callee&& runnable)
		{
			promise.set_value(runnable());
		}
		template <typename Result, typename Callee, typename std::enable_if_t<std::is_void<Result>::value, int> = 1>
		static void execute(std::promise<void>&& promise, Callee&& runnable)
		{
			runnable();
			promise.set_value();
		}
	};

}//namespace tools
}//namespace pisk

