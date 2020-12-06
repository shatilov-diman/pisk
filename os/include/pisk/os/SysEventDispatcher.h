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

#include <pisk/utils/noncopyable.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentPtr.h>
#include <pisk/os/MainLoopRemoteTasks.h>

#include <mutex>
#include <map>

namespace pisk
{
namespace os
{
	template <typename Event>
	class SysEventHandlerBase :
		public utils::noncopyable
	{
		template <typename E>
		friend class SysEventHandler;

		SysEventHandlerBase()
		{}

		~SysEventHandlerBase()
		{}

	public:
		virtual bool handle(const Event& event) = 0;
	};

	template <typename Event>
	class SysEventDispatcher :
		public core::Component
	{
		template <typename E>
		friend class SysEventHandler;

		MainLoopRemoteTasksPtr main_loop_remote_tasks;

		mutable std::recursive_mutex mutex;
		std::map<void*, SysEventHandlerBase<Event>*> handlers;

	public:

		SysEventDispatcher(const MainLoopRemoteTasksPtr& _main_loop_remote_tasks) :
			main_loop_remote_tasks(_main_loop_remote_tasks)
		{
			if (main_loop_remote_tasks == nullptr)
				throw infrastructure::NullPointerException();
		}

		virtual void release() final override
		{
			pisk::logger::info("sys_event_dispatcher", "Destroing");
			delete this;
		}

	private:
		void subscribe(SysEventHandlerBase<Event>* handler) threadsafe
		{
			pisk::logger::debug("sys_event_dispatcher", "subscribe {}", handler);

			std::lock_guard<std::recursive_mutex> guard(mutex);
			handlers[handler] = handler;
		}

		void unsubscribe(SysEventHandlerBase<Event>* handler) threadsafe
		{
			pisk::logger::debug("sys_event_dispatcher", "unsubscribe {}", handler);

			std::lock_guard<std::recursive_mutex> guard(mutex);
			handlers[handler] = nullptr;
		}

	public:
		template <typename Callee>
		auto run_from_dispatcher_thread_sync(Callee&& runnable) threadsafe -> decltype(runnable())
		{
			return main_loop_remote_tasks->run_from_main_thread_sync(std::forward<Callee>(runnable));
		}

		template <typename Callee>
		auto run_from_dispatcher_thread_async(Callee&& runnable) threadsafe -> decltype(main_loop_remote_tasks->run_from_main_thread_async(std::forward<Callee>(runnable)))
		{
			return main_loop_remote_tasks->run_from_main_thread_async(std::forward<Callee>(runnable));
		}

	public:
		constexpr static const char* uid = "sys_event_dispatcher";

		bool dispatch(const Event& event) threadsafe
		{
			bool result = false;
			std::lock_guard<std::recursive_mutex> guard(mutex);
			for (auto iter = handlers.begin(); iter != handlers.end();)
			{
				if (iter->second == nullptr)
					iter = handlers.erase(iter);
				else
				{
					result |= iter->second->handle(event);
					++iter;
				}
			}
			return result;
		}
	};
	template <typename Event>
	using SysEventDispatcherPtr = tools::InterfacePtr<SysEventDispatcher<Event>>;


	template <typename Event>
	class SysEventHandler :
		public SysEventHandlerBase<Event>
	{
		SysEventDispatcherPtr<Event> dispatcher;
	public:
		explicit SysEventHandler(SysEventDispatcherPtr<Event> dispatcher):
			dispatcher(dispatcher)
		{
			dispatcher->subscribe(this);
		}
		~SysEventHandler()
		{
			dispatcher->unsubscribe(this);
		}

		SysEventDispatcherPtr<Event> get_dispatcher() const
		{
			return dispatcher;
		}

		template <typename Callee>
		auto run_from_dispatcher_thread_sync(Callee&& runnable) -> decltype(runnable()) threadsafe
		{
			return dispatcher->run_from_dispatcher_thread_sync(std::forward<Callee>(runnable));
		}

		template <typename Callee>
		auto run_from_dispatcher_thread_async(Callee&& runnable) -> decltype(dispatcher->run_from_dispatcher_thread_async(std::forward<Callee>(runnable))) threadsafe
		{
			return dispatcher->run_from_dispatcher_thread_async(std::forward<Callee>(runnable));
		}
	};
}
}

