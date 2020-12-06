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


#include <pisk/tools/ComponentsLoader.h>

#include <pisk/os/unix/cui/SysEvent.h>

#include <memory>

using namespace pisk::tools;

SafeComponentPtr __cdecl syseventdispatcher_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&syseventdispatcher_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto remote_tasks = temp_sl.get<pisk::os::MainLoopRemoteTasks>();
	if (remote_tasks == nullptr)
	{
		pisk::logger::critical("loop_factory", "Unable to locate MainLoopRemoteTasks");
		throw pisk::infrastructure::NullPointerException();
	}

	return factory.make<pisk::os::OSSysEventDispatcher>(remote_tasks);
}

