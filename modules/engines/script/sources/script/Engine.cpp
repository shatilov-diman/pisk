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

#include <pisk/tools/ComponentsLoader.h>
#include <pisk/tools/MainLoop.h>
#include <pisk/system/ResourceManager.h>
#include <pisk/system/EngineComponentFactory.h>
#include <pisk/system/EngineStrategyBL.h>

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
			pisk::logger::warning("script", "Unable to locate '{}' component", name);
			continue;
		}
		auto service = component.cast<pisk::script::Service2Go>();
		if (service == nullptr)
		{
			pisk::logger::warning("script", "Unable to get Service2Go interface from '{}' component", name);
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

	auto main_loop = temp_sl.get<pisk::tools::MainLoop>();
	if (main_loop == nullptr)
		return {};

	pisk::script::Service2GoContainer services2go = get_services2go(temp_sl, config);

	using EngineStrategyType =
	pisk::script::S2GEngineStrategy<
		pisk::script::LogEngineStrategy<
			pisk::script::PushChangesEngineStrategy<
				pisk::script::StopAppEngineStrategy<
					pisk::script::LoadSceneEngineStrategy<
						pisk::system::EngineStrategyBL<
							pisk::script::EngineStrategy
	> > > > > >;

	return engine_factory->make_engine(
		factory,
		[services2go, main_loop, resource_manager, config](pisk::system::PatchRecipient& patch_recipient) {
			return std::make_unique<EngineStrategyType>(services2go, main_loop, resource_manager, patch_recipient, config);
		}
	);
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_script_engine_factory()
{
	return &script_engine_factory;
}

