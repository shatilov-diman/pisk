// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module os of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional restriction according to GPLv3 pt 7:
// b) required preservation author attributions;
// c) required preservation links to original sources
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

#include <pisk/system/ResourceManager.h>
#include <pisk/system/PropertyLoader.h>

#include "NativeFileResourcePack.h"
#include "OsAppInstance.h"

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
	infrastructure::Logger::get().error("framework", "Unable to load 'components'");
	throw;
}
catch (infrastructure::Exception&)
{
	infrastructure::Logger::get().error("framework", "Something went wrong");
	throw;
}

void common_configure_components(const tools::ServiceRegistry&)
{
}

void common_init_logger(std::unique_ptr<infrastructure::LogStorage>&& log_storage)
{
	infrastructure::Logger::get().set_log_level(infrastructure::Logger::Level::Debug);
	infrastructure::Logger::get().set_log_storage(std::move(log_storage));
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

pisk::tools::SafeComponentPtr common_make_os_app_instance_component(const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&common_make_os_app_instance_component), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	return factory.make<pisk::os::impl::OsAppInstance>();
}

