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
		virtual ResourcePtr load(const std::string& rid, const std::string& resource_type) threadsafe const final override
		{
			logger::debug("resource_manager", "Loading resource '{}'", rid);
			infrastructure::DataStreamPtr stream = packs.open(rid);
			if (stream == nullptr)
				throw infrastructure::NullPointerException();
			ResourceLoaderPtr loader = loader_registry.get_loader(resource_type, stream);
			logger::debug("resource_manager", "Loader was found for resource '{}'", rid);
			ResourcePtr resource = loader->load(std::move(stream));
			logger::debug("resource_manager", "Resource '{}' was successfully loaded", rid);
			return resource;
 		}
		virtual system::ResourcePackManager& get_pack_manager() final override
		{
			return packs;
		}
		virtual system::ResourceLoaderRegistry& get_loader_registry() final override
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

