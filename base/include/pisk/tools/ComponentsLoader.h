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
#include "../infrastructure/DataStream.h"
#include "../utils/property_tree.h"

#include "ComponentPtr.h"
#include "ServiceRegistry.h"
#include "InstanceFactory.h"
#include "Releasable.h"

#include <memory>
#include <vector>

namespace pisk
{
namespace tools
{
namespace components
{
	struct Description
	{
		std::string type;
		std::string name;
		std::string module;
		std::string factory;
		utils::property config;
		std::vector<std::string> dependencies;

		bool operator == (const Description& desc) const {
			return type == desc.type and name == desc.name
				and module == desc.module and factory == desc.factory;
		}
	};
	using DescriptionsList = std::vector<Description>;

	using ComponentFactory = SafeComponentPtr (__cdecl *)(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const utils::property& config);
	using ComponentFactoryFn = std::function<typename std::remove_pointer<ComponentFactory>::type>;

	using ComponentFactoryGetter = ComponentFactory (__cdecl *)();
	using ComponentFactoryGetterFn = std::function<typename std::remove_pointer<ComponentFactoryGetter>::type>;

	class Parser : public utils::noncopyable
	{
	public:
		static DescriptionsList parse(const infrastructure::DataStream& data) threadsafe noexcept;

	private:
		static utils::property load_components_list(const infrastructure::DataStream& data);
		static DescriptionsList parse_components_list(const utils::property& list);
	};


	class Loader : public utils::noncopyable
	{
	public:
		static void load(
			const DescriptionsList& descriptions,
			std::function<infrastructure::ModulePtr (const std::string& name)> get_module,
			std::function<SafeComponentPtr (ComponentFactoryFn factory, infrastructure::ModulePtr, const utils::property&)> load_component,
			std::function<void (const std::string& name, SafeComponentPtr component)> store_component
		) threadsafe;
	};
}
}
}

