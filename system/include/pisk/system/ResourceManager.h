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


#pragma once

#include <pisk/defines.h>
#include <pisk/infrastructure/Exception.h>
#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentPtr.h>

#include "ResourcePackManager.h"
#include "ResourceLoaderRegistry.h"

namespace pisk
{
namespace system
{
	class ResourceManager :
		public core::Component
	{
		virtual ResourcePtr load(const std::string& rid, const std::string& resource_type) threadsafe const = 0;
	public:
		constexpr static const char* uid = "resource_manager";

		virtual ResourcePackManager& get_pack_manager() = 0;

		virtual ResourceLoaderRegistry& get_loader_registry() = 0;

		template <typename ResourceType>
		resource_ptr<ResourceType> load(const std::string& rid) const threadsafe
	       	{
			ResourcePtr resource = load(rid, ResourceType::resource_type);
			if (auto casted_resource = std::dynamic_pointer_cast<ResourceType>(resource))
				return casted_resource;
			infrastructure::Logger::get().error("resource_manager", "Resource '%s' failed to cast to '%s' resource", rid.c_str(), typeid(ResourceType).name());
			throw infrastructure::InvalidArgumentException();
		}
	};
	using ResourceManagerPtr = tools::InterfacePtr<ResourceManager>;
}
}

