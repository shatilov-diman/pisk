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


#pragma once

#include "../defines.h"
#include "../utils/noncopyable.h"

#include "ComponentPtr.h"
#include "OsAppInstance.h"
#include "ComponentsLoader.h"

#include "ServiceRegistry.h"

#include <vector>
#include <iostream>

namespace pisk
{
namespace tools
{
	namespace impl
	{
		class Application;
	}

	struct OsComponent
	{
		utils::keystring name;
		components::ComponentFactoryFn factory;
	};
	using OsComponentList = std::vector<OsComponent>;

	struct AppConfigurator
	{
		OsComponentList os_components;

		std::function <void (const tools::ServiceRegistry& sl)> configure_core_components;
		std::function <components::DescriptionsList (const tools::ServiceRegistry& sl)> components_list_provider;
		std::function <void (const tools::ServiceRegistry& sl)> configure_components;

		std::function <infrastructure::ModulePtr (const std::string& basename)> module_loader;
	};

	class Application :
		public Releasable,
		public utils::noncopyable
	{
	public:
		virtual void run(const AppConfigurator& configurator) = 0;

		virtual void stop() threadsafe noexcept = 0;
	};
	using ApplicationPtr = tools::unique_relesable_ptr<Application>;

	ApplicationPtr EXPORT make_application();
}
}

