// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module system of the project pisk.
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


#include "EngineComponentFactory.h"

#include <pisk/tools/ComponentsLoader.h>

#include <memory>

pisk::system::PatchPortalPtr CreatePatchPortal();

using namespace pisk::tools;

SafeComponentPtr __cdecl engine_component_factory_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&engine_component_factory_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	pisk::system::PatchPortalPtr&& portal = CreatePatchPortal();
	return factory.make<pisk::system::impl::EngineComponentFactory>(std::move(portal));
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_engine_component_factory_factory()
{
	static_assert(std::is_convertible<decltype(&get_engine_component_factory_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &engine_component_factory_factory;
}
