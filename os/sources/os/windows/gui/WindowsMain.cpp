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


#include "../../common.h"
#include "../../ConsoleLogger.h"

#include <pisk/tools/MainLoop.h>
#include <pisk/os/WindowManager.h>
#include <pisk/os/MainLoopRemoteTasks.h>

#include <pisk/os/windows/gui/SysEvent.h>
#include <pisk/os/windows/gui/OsAppInstance.h>

EXPORT pisk::infrastructure::ModulePtr CreateModule(const std::string& basename);

pisk::tools::SafeComponentPtr __cdecl make_os_app_instance_component(const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	HINSTANCE hinstance = ::GetModuleHandle(NULL);
	return factory.make<pisk::os::windows_gui::OsAppInstance>(hinstance);
}
pisk::tools::SafeComponentPtr __cdecl common_mainloop_remote_tasks_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl syseventdispatcher_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl window_manager_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl main_loop_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);


extern "C"
EXPORT void run_application()
{
	pisk::os::common_init_logger(std::make_unique<pisk::os::impl::ConsoleLogger>());

	const pisk::tools::OsComponentList components {
		{pisk::tools::OsAppInstance::uid, &make_os_app_instance_component},
		{pisk::os::MainLoopRemoteTasks::uid, &common_mainloop_remote_tasks_factory},
		{pisk::os::OSSysEventDispatcher::uid, &syseventdispatcher_factory},
		{pisk::tools::MainLoop::uid, &main_loop_factory},
		{pisk::os::WindowManager::uid, &window_manager_factory},
	};

	pisk::os::common_run_application({
		components,
		&pisk::os::common_configure_core_components,
		&pisk::os::common_load_components_list,
		&pisk::os::common_configure_components,
		&CreateModule,
	});
}

