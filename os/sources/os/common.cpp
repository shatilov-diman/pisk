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
#include <pisk/utils/json_utils.h>
#include <pisk/utils/property_tree.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/infrastructure/Exception.h>

#include <pisk/tools/Application.h>
#include <pisk/os/MainLoopRemoteTasks.h>

#include <pisk/system/ResourceManager.h>
#include <pisk/system/PropertyLoader.h>

#include "NativeFileResourcePack.h"
#include "OsAppInstance.h"

#include <ctime>

namespace pisk
{
namespace os
{

void common_configure_core_components(const tools::ServiceRegistry& temp_sl)
{
	auto res_manager = temp_sl.get<system::ResourceManager>();
	res_manager->get_pack_manager().set_pack("", std::make_unique<FileResourcePack>());
}

tools::components::DescriptionsList common_load_components_list(const tools::ServiceRegistry& temp_sl)
try
{
	tools::components::DescriptionsList out;

	auto res_manager = temp_sl.get<system::ResourceManager>();
	const utils::property& components = res_manager->load<system::PropertyTreeResource>("components")->get();
	for (const auto& cmp : components)
	{
		if (cmp.is_dictionary())
		{
			out.push_back({
				cmp["type"].as_string(),
				cmp["name"].as_string(),
				cmp["module"].as_string(),
				cmp["factory"].as_string(),
				cmp["config"]
			});
			if (not cmp["dependencies"].is_none())
				for(const auto& libname : cmp["dependencies"].as_array())
					out.back().dependencies.push_back(libname.second.as_string());
		}
	}
	return out;
}
catch (system::ResourceNotFound&)
{
	logger::error("framework", "Unable to load 'components'");
	throw;
}
catch (infrastructure::Exception&)
{
	logger::error("framework", "Something went wrong");
	throw;
}

void common_configure_components(const tools::ServiceRegistry&)
{
}

void common_init_logger(std::unique_ptr<infrastructure::LogStorage>&& log_storage)
{
	logger::set_log_level(infrastructure::Logger::Level::Debug);
	logger::set_log_storage(std::move(log_storage));
}

void common_run_application(const tools::AppConfigurator& configurator)
{
	auto salt = time(0);
	std::srand(static_cast<unsigned long>(salt));

	tools::ApplicationPtr app = tools::make_application();
	app->run(configurator);
}

}
}

pisk::tools::SafeComponentPtr __cdecl common_make_os_app_instance_component(const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&common_make_os_app_instance_component), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	return factory.make<pisk::os::impl::OsAppInstance>();
}

pisk::tools::SafeComponentPtr __cdecl common_mainloop_remote_tasks_factory(const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&common_mainloop_remote_tasks_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	return factory.make<pisk::os::MainLoopRemoteTasks>();
}