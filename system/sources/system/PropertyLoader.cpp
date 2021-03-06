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
#include <pisk/utils/json_utils.h>
#include <pisk/tools/ComponentsLoader.h>

#include <pisk/system/PropertyLoader.h>

namespace pisk
{
namespace system
{
namespace impl
{
	class PropertyTreeResource :
		public system::PropertyTreeResource
	{
		utils::property prop;

		virtual utils::property get() threadsafe const noexcept final override
		{
			return prop;
		}

	public:
		explicit PropertyTreeResource(utils::property&& prop):
			prop(std::move(prop))
		{}
	};

	class JsonToPropertyTreeResourceLoader :
		public ResourceLoader
	{
		virtual void release() final override
		{
			delete this;
		}

		virtual bool can_load(const infrastructure::DataStreamPtr&) const noexcept threadsafe final override
		{
			return true;
		}

		virtual ResourcePtr load(infrastructure::DataStreamPtr&& stream) const threadsafe final override
		{
			if (stream == nullptr)
				throw infrastructure::NullPointerException();
			auto&& prop = utils::json::parse_json_to_property(*stream);
			return std::make_shared<PropertyTreeResource>(std::move(prop));
		}
	};

}
}
}

using namespace pisk::tools;

SafeComponentPtr __cdecl property_tree_loader_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&property_tree_loader_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto resource_manager = temp_sl.get<pisk::system::ResourceManager>();
	if (resource_manager == nullptr)
		throw pisk::infrastructure::NullPointerException();

	auto resource_loader = factory.make<pisk::system::impl::JsonToPropertyTreeResourceLoader>();

	resource_manager->get_loader_registry().register_resource_loader(
		pisk::system::PropertyTreeResource::resource_type, resource_loader
	);
	return resource_loader;
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_property_tree_loader_factory()
{
	static_assert(std::is_convertible<decltype(&get_property_tree_loader_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &property_tree_loader_factory;
}

