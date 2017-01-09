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

