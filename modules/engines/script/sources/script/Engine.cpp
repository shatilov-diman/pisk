// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module script of the project pisk.
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

#include <pisk/tools/ComponentsLoader.h>
#include <pisk/system/ResourceManager.h>
#include <pisk/system/EngineComponentFactory.h>

#include "EngineStrategy.h"
#include "EngineStrategyEx.h"
#include "Service2GoEngineStrategy.h"

using namespace pisk::tools;

static pisk::script::Service2GoContainer get_services2go(const ServiceRegistry& temp_sl, const pisk::utils::property& config)
{
	pisk::script::Service2GoContainer services2go;
	for (const pisk::utils::property& name_prop : config["service2go"])
	{
		const auto& name = name_prop.as_keystring();
		auto component = temp_sl.get<pisk::core::Component>(name);
		if (component == nullptr)
		{
			pisk::infrastructure::Logger::get().warning("script", "Unable to locate '%s' component", name.c_str());
			continue;
		}
		auto service = component.cast<pisk::script::Service2Go>();
		if (service == nullptr)
		{
			pisk::infrastructure::Logger::get().warning("script", "Unable to get Service2Go interface from '%s' component", name.c_str());
			continue;
		}
		services2go[name] = service;
	}
	return services2go;
}

SafeComponentPtr __cdecl script_engine_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property& config)
{
	static_assert(std::is_convertible<decltype(&script_engine_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto resource_manager = temp_sl.get<pisk::system::ResourceManager>();
	if (resource_manager == nullptr)
		return {};

	auto engine_factory = temp_sl.get<pisk::system::EngineComponentFactory>();
	if (engine_factory == nullptr)
		return {};

	pisk::script::Service2GoContainer services2go = get_services2go(temp_sl, config);

	using EngineStrategyType =
	pisk::script::S2GEngineStrategy<
		pisk::script::LogEngineStrategy<
			pisk::script::PushChangesEngineStrategy<
				pisk::script::EngineStrategy
	> > >;

	return engine_factory->make_engine(
		factory,
		[services2go, resource_manager, config](pisk::system::PatchRecipient& patch_recipient) {
			return std::make_unique<EngineStrategyType>(services2go, resource_manager, patch_recipient, config);
		}
	);
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_script_engine_factory()
{
	return &script_engine_factory;
}

