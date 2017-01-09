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


#include <pisk/defines.h>
#include <pisk/infrastructure/Logger.h>
#include <pisk/utils/algorithm_utils.h>
#include <pisk/tools/ComponentsLoader.h>

#include <pisk/system/ResourceManager.h>

#include "ResourcePackManager.h"
#include "ResourceLoaderRegistry.h"

#include <memory>

namespace pisk
{
namespace system
{
namespace impl
{
	class ResourceManager :
		public system::ResourceManager
	{
		PackManager packs;
		ResourceLoaderRegistry loader_registry;

		virtual void release() final override
		{
			delete this;
		}
		virtual ResourcePtr load(const std::string& rid, const std::string& resource_type) threadsafe const override final
		{
			infrastructure::Logger::get().debug("resource_manager", "Loading resource '%s'", rid.c_str());
			infrastructure::DataStreamPtr stream = packs.open(rid);
			if (stream == nullptr)
				throw infrastructure::NullPointerException();
			ResourceLoaderPtr loader = loader_registry.get_loader(resource_type, stream);
			infrastructure::Logger::get().debug("resource_manager", "Loader was found for resource '%s'", rid.c_str());
			ResourcePtr resource = loader->load(std::move(stream));
			infrastructure::Logger::get().debug("resource_manager", "Resource '%s' was successfully loaded", rid.c_str());
			return resource;
 		}
		virtual system::ResourcePackManager& get_pack_manager()
		{
			return packs;
		}
		virtual system::ResourceLoaderRegistry& get_loader_registry()
		{
			return loader_registry;
		}
	};
}
}
}

std::shared_ptr<pisk::system::ResourceManager> CreateResourceManager()
{
	return pisk::tools::make_shared_releasable<pisk::system::impl::ResourceManager>();
}

using namespace pisk::tools;

SafeComponentPtr __cdecl resource_manager_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&resource_manager_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	return factory.make<pisk::system::impl::ResourceManager>();
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_resource_manager_factory()
{
	static_assert(std::is_convertible<decltype(&get_resource_manager_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &resource_manager_factory;
}

