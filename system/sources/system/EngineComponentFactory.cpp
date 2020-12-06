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
#include <pisk/tools/MainLoop.h>

#include "EngineComponentFactory.h"
#include "EngineSynchronizer.h"

#include <memory>

using namespace pisk::utils;
using namespace pisk::tools;
using namespace pisk::system;

SafeComponentPtr __cdecl engine_component_factory_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const property&)
{
	static_assert(std::is_convertible<decltype(&engine_component_factory_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto main_loop = temp_sl.get<MainLoop>(MainLoop::uid);
	if (main_loop == nullptr)
		return {};

	auto engine_factory = factory.make<subscribtions_holder_proxy<impl::EngineComponentFactory>>();

	engine_factory->store_subscribtion(main_loop->on_begin_loop.subscribe(std::bind(
		&impl::EngineComponentFactory::start, engine_factory.get()//use raw pointer to avoid issue with cyclic links
	)));
	engine_factory->store_subscribtion(main_loop->on_end_loop.subscribe(std::bind(
		&impl::EngineComponentFactory::stop, engine_factory.get()//use raw pointer to avoid issue with cyclic links
	)));

	return engine_factory;
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_engine_component_factory_factory()
{
	static_assert(std::is_convertible<decltype(&get_engine_component_factory_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &engine_component_factory_factory;
}
