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
#include <pisk/system/EngineComponentFactory.h>

#include "EngineStrategy.h"

using namespace pisk::tools;

SafeComponentPtr __cdecl io_engine_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&io_engine_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto engine_factory = temp_sl.get<pisk::system::EngineComponentFactory>();
	if (engine_factory == nullptr)
		return {};

	auto keyboard = temp_sl.get<pisk::io::Keyboard>();
	auto mouse = temp_sl.get<pisk::io::Mouse>();
	//nullptr is ok!

	return engine_factory->make_engine(
		factory,
		[keyboard, mouse](pisk::system::PatchRecipient& patch_recipient) {
			return std::make_unique<pisk::io::EngineStrategy>(keyboard, mouse, patch_recipient);
		}
	);
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_io_engine_factory()
{
		return &io_engine_factory;
}

