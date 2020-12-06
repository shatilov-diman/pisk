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


#include <pisk/tools/MainLoop.h>
#include <pisk/system/ResourceManager.h>

#include <pisk/os/WindowManager.h>
#include <pisk/os/MainLoopRemoteTasks.h>

#include <pisk/os/android/jni_os_headers.h>
#include <pisk/os/android/os_app_instance.h>
#include <pisk/os/android/SysEvent.h>

#include "../../common.h"
#include "../../NativeFileResourcePack.h"

#include "AndroidAssetPack.h"
#include "AndroidLogger.h"

namespace pisk
{
namespace os
{
namespace impl
{

pisk::infrastructure::ModulePtr create_module(const std::string& basename, android_app* application);

static void configure_core_components(const tools::ServiceRegistry& temp_sl, struct android_app* app)
{
	if (app == nullptr)
		throw infrastructure::NullPointerException();

	auto res_manager = temp_sl.get<system::ResourceManager>();

	auto&& external_data_files_pack = std::make_unique<FileResourcePack>(app->activity->externalDataPath);
	res_manager->get_pack_manager().set_pack("external_data", std::move(external_data_files_pack));

	auto&& data_files_pack = std::make_unique<FileResourcePack>(app->activity->internalDataPath);
	res_manager->get_pack_manager().set_pack("data", std::move(data_files_pack));

	auto&& assets_pack = std::make_unique<impl::AndroidAssetsPack>(app->activity->assetManager);
	res_manager->get_pack_manager().set_pack("assets", std::move(assets_pack));

}

}
}
}

pisk::tools::SafeComponentPtr __cdecl make_os_app_instance_component(struct android_app* app, const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	return factory.make<pisk::os::android::OsAppInstance>(app);
}

pisk::tools::SafeComponentPtr __cdecl common_mainloop_remote_tasks_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl syseventdispatcher_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl window_manager_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);
pisk::tools::SafeComponentPtr __cdecl main_loop_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&);

extern "C"
EXPORT void android_main(struct android_app* android_app)
{
	app_dummy();

	// Uncomment for wait gdb
	//std::raise(SIGSTOP);

	pisk::os::common_init_logger(std::make_unique<pisk::os::impl::AndroidConsoleLogger>());

	const pisk::tools::OsComponentList components {
		{pisk::tools::OsAppInstance::uid, std::bind(&make_os_app_instance_component, android_app, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
		{pisk::os::MainLoopRemoteTasks::uid, &common_mainloop_remote_tasks_factory},
		{pisk::os::OSSysEventDispatcher::uid, &syseventdispatcher_factory},
		{pisk::tools::MainLoop::uid, &main_loop_factory},
		{pisk::os::WindowManager::uid, &window_manager_factory},
	};

	pisk::os::common_run_application({
		components,
		std::bind(
			&pisk::os::impl::configure_core_components,
			std::placeholders::_1,
			android_app
		),
		&pisk::os::common_load_components_list,
		&pisk::os::common_configure_components,
		std::bind(
			&pisk::os::impl::create_module,
			std::placeholders::_1,
			android_app
		),
	});
}

