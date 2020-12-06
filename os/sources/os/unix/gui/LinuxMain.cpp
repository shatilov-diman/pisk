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

#include <pisk/os/unix/gui/x11.h>
#include <pisk/os/unix/gui/SysEvent.h>
#include <pisk/os/unix/gui/OsAppInstance.h>

EXPORT pisk::infrastructure::ModulePtr CreateModule(const std::string& basename);

pisk::tools::SafeComponentPtr __cdecl make_os_app_instance_component(pisk::os::XDisplay display, const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	return factory.make<pisk::os::unix_gui::OsAppInstance>(display);
}
pisk::tools::SafeComponentPtr __cdecl common_mainloop_remote_tasks_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl syseventdispatcher_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl window_manager_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl main_loop_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);


int x_error_handler(pisk::os::XDisplay display, XErrorEvent* event)
{
	char buffer[128];
	::XGetErrorText(display, event->error_code, buffer, sizeof(buffer));

	pisk::logger::error("X System", "Error message: {}", buffer);

	return 0;
}

extern "C"
void run_application()
{
	pisk::os::common_init_logger(std::make_unique<pisk::os::impl::ConsoleLogger>());

	::XSetErrorHandler(&x_error_handler);
	::XInitThreads();

	pisk::os::XDisplay display = ::XOpenDisplay(NULL);
	if (display == nullptr)
	{
		pisk::logger::critical("loop_factory", "Unable to connect to X DISPLAY");
		throw pisk::infrastructure::InitializeError();
	}

	const pisk::tools::OsComponentList components {
		{
			pisk::tools::OsAppInstance::uid,
			std::bind(
				&make_os_app_instance_component,
				display,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
			)
		},
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

	::XCloseDisplay(display);
}

