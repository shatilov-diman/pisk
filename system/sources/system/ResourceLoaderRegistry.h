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

#include <pisk/system/ResourceLoaderRegistry.h>

#include <map>
#include <mutex>
#include <memory>

namespace pisk
{
namespace system
{
namespace impl
{
	class ResourceLoaderRegistry :
		public system::ResourceLoaderRegistry
	{
		mutable std::mutex mutex;
		std::map<std::string, std::set<ResourceLoaderPtr>> loaders;

		virtual void release() final override
		{
			delete this;
		}

		virtual void register_resource_loader(const std::string& resource_type, ResourceLoaderPtr loader) final override
		{
			if (loader == nullptr)
				throw infrastructure::NullPointerException();
			std::unique_lock<std::mutex> guard(mutex);
			loaders[resource_type].insert(loader);
		}

		std::set<ResourceLoaderPtr> get_loaders_for_type(const std::string& resource_type) const
		{
			std::unique_lock<std::mutex> guard(mutex);
			auto set = loaders.find(resource_type);
			if (set == loaders.end())
				return {};
			return set->second;
		}

	public:
		ResourceLoaderPtr get_loader(const std::string& resource_type, const infrastructure::DataStreamPtr& stream) const threadsafe
		{
			if (stream == nullptr)
				throw infrastructure::NullPointerException();
			const auto& set = get_loaders_for_type(resource_type);
			for (const auto& loader : set)
				if (loader->can_load(stream))
					return loader;
			throw system::ResourceFormatNotSupported();
		}
	};
}
}
}

