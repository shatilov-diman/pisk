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

#include <pisk/tools/ComponentPtr.h>
#include <pisk/tools/RemoteTaskList.h>
#include <pisk/infrastructure/Logger.h>

#include <atomic>
#include <thread>

namespace pisk
{
namespace os
{
	class MainLoopRemoteTasks :
		public core::Component
	{
		tools::RemoteTaskExecutor remote_task_list;

	public:
		constexpr static const char* uid = "main_loop_remote_tasks";

		virtual void release() final override
		{
			pisk::logger::info(uid, "Destroing");
			delete this;
		}

		void init()
		{
			pisk::logger::info(uid, "Initializing");
			remote_task_list.init();
		}

		void execute()
		{
			remote_task_list.execute_remote_tasks();
		}

		void deinit()
		{
			pisk::logger::info(uid, "Deinitializing");
			remote_task_list.deinit();
		}

		template <typename Callee>
		auto run_from_main_thread_sync(Callee&& runnable) -> decltype(runnable()) threadsafe
		{
			return remote_task_list.push_remote_task_sync(std::forward<Callee>(runnable));
		}

		template <typename Callee>
		auto run_from_main_thread_async(Callee&& runnable) -> decltype(remote_task_list.push_remote_task_async(std::forward<Callee>(runnable))) threadsafe
		{
			return remote_task_list.push_remote_task_async(std::forward<Callee>(runnable));
		}
	};
	using MainLoopRemoteTasksPtr = tools::InterfacePtr<MainLoopRemoteTasks>;
}
}

