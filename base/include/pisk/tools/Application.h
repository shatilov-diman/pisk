// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
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


#pragma once

#include "../defines.h"
#include "../utils/noncopyable.h"

#include "ComponentPtr.h"
#include "OsAppInstance.h"
#include "EventsContainer.h"
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

		virtual EventsContainer& get_events() threadsafe noexcept = 0;
	};
	using ApplicationPtr = tools::unique_relesable_ptr<Application>;

	ApplicationPtr EXPORT make_application();
}
}

