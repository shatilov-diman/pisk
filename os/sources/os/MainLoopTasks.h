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
#include <pisk/infrastructure/Logger.h>

#include <pisk/os/MainLoopRemoteTasks.h>

namespace pisk
{
namespace os
{

template <typename TMainLoopBase>
class MainLoopTasks :
	public TMainLoopBase
{
	MainLoopRemoteTasksPtr remote_tasks;

public:

	template <typename ... Args>
	MainLoopTasks(const MainLoopRemoteTasksPtr remote_tasks, Args&& ... args) :
		TMainLoopBase(std::forward<Args>(args)...),
		remote_tasks(remote_tasks)
	{}

	virtual void init() override
	{
		logger::info("main_loop", "MainLoopTasks::init");
		remote_tasks->init();
		TMainLoopBase::init();
	}

	virtual void common_loop() override
	{
		TMainLoopBase::common_loop();
		remote_tasks->execute();
	}

	virtual void deinit() override
	{
		logger::info("main_loop", "MainLoopTasks::deinit");
		TMainLoopBase::deinit();
		remote_tasks->deinit();
	}
};

}//namespace os
}//namespace pisk

