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

#include <pisk/tools/MainLoop.h>
#include <pisk/tools/ComponentsLoader.h>
#include <pisk/system/ResourceManager.h>
#include <pisk/system/EngineComponentFactory.h>

#include <pisk/os/WindowManager.h>

#include "EngineStrategy.h"
#include "Engine.h"

using namespace pisk::tools;

namespace pisk {
namespace graphic {

	ResourceLoaderPtr make_resource_loader(const system::ResourceManagerPtr& resource_manager);

}//namespace graphic
}//namespace pisk

SafeComponentPtr __cdecl graphic_engine_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property& config)
{
	static_assert(std::is_convertible<decltype(&graphic_engine_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto engine_factory = temp_sl.get<pisk::system::EngineComponentFactory>();
	auto resource_manager = temp_sl.get<pisk::system::ResourceManager>();
	auto window_manager = temp_sl.get<pisk::os::WindowManager>();
	if (engine_factory == nullptr or resource_manager == nullptr or window_manager == nullptr)
		return {};

	return engine_factory->make_engine(
		factory,
		[&](pisk::system::PatchRecipient& patch_recipient) {
			return std::make_unique<pisk::graphic::EngineStrategy>(
				config,
				window_manager,
				patch_recipient,
				pisk::graphic::make_resource_loader(resource_manager)
			);
		}
	);
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_graphic_engine_factory()
{
	return &graphic_engine_factory;
}

