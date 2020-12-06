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


#include <pisk/defines.h>
#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentsLoader.h>

#include "../../MainLoopBase.h"
#include "../../MainLoopSignal.hpp"
#include "../../MainLoopTasks.h"
#include "KeyboardProcessor.h"

pisk::tools::SafeComponentPtr __cdecl main_loop_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&main_loop_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto dispatcher = temp_sl.get<pisk::os::OSSysEventDispatcher>();
	if (dispatcher == nullptr)
	{
		pisk::logger::critical("loop_factory", "Unable to locate SysEventDispatcher");
		throw pisk::infrastructure::NullPointerException();
	}
	auto remote_tasks = temp_sl.get<pisk::os::MainLoopRemoteTasks>();
	if (remote_tasks == nullptr)
	{
		pisk::logger::critical("loop_factory", "Unable to locate MainLoopRemoteTasks");
		throw pisk::infrastructure::NullPointerException();
	}

	using MainLoop = pisk::os::MainLoopSignal<pisk::os::MainLoopTasks<pisk::os::MainLoopBase>>;

	return factory.make<MainLoop>(
		remote_tasks,
		std::make_unique<pisk::os::impl::KeyboardProcessor>(dispatcher)
	);
}

