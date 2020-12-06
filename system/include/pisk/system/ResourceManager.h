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
			logger::error("resource_manager", "Resource '{}' failed to cast to '{}' resource", rid, typeid(ResourceType).name());
			throw infrastructure::InvalidArgumentException();
		}
	};
	using ResourceManagerPtr = tools::InterfacePtr<ResourceManager>;
}
}

