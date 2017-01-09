// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module lua of the project pisk.
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

#include "LuaLoader.h"

using namespace pisk::tools;
using namespace pisk::script;

namespace pisk
{
namespace script
{
namespace loaders
{
	script::ScriptPtr make_script_instance(const infrastructure::DataBuffer& data);
}
}
}

SafeComponentPtr __cdecl lua_loader_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&lua_loader_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto resource_manager = temp_sl.get<pisk::system::ResourceManager>();
	if (resource_manager == nullptr)
		throw pisk::infrastructure::NullPointerException();

	auto resource_loader = factory.make<pisk::script::loaders::LuaLoader>(&pisk::script::loaders::make_script_instance);
	resource_manager->get_loader_registry().register_resource_loader(
		pisk::script::Resource::resource_type, resource_loader
	);
	return resource_loader;
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_lua_loader_factory()
{
	static_assert(std::is_convertible<decltype(&get_lua_loader_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &lua_loader_factory;
}

