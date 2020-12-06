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


#include <pisk/system/ResourceManager.h>

#include "../resources/ShaderResource.h"

namespace pisk
{
namespace graphic
{
namespace loaders
{
	class ShaderSource :
		public graphic::ShaderSource
	{
		const utils::keystring type;
		const utils::keystring source;
	public:
		ShaderSource(const utils::keystring& _type, const utils::keystring& _source):
			type(_type),
			source(_source)
		{}

		virtual const utils::keystring& get_type() const final override
		{
			return type;
		}

		virtual const utils::keystring& get_source() const final override
		{
			return source;
		}
	};

	class ShaderResource :
		public graphic::ShaderResource
	{
		infrastructure::DataBuffer data;

		virtual ShaderSourcePtr load(const utils::keystring& type) threadsafe const noexcept final override
		{
			const std::string content{data.begin(), data.end()};
			return std::make_unique<ShaderSource>(type, utils::keystring{content});
		}

	public:
		ShaderResource(infrastructure::DataStreamPtr&& stream):
			data(stream->readall())
		{}
	};

	class ShaderLoader :
		public system::ResourceLoader
	{
		virtual void release() final override
		{
			delete this;
		}

		virtual bool can_load(const infrastructure::DataStreamPtr&) const noexcept threadsafe final override
		{
			return true;
		}

		virtual system::ResourcePtr load(infrastructure::DataStreamPtr&& stream) const noexcept threadsafe final override
		{
			if (stream == nullptr)
				return nullptr;
			return std::make_shared<ShaderResource>(std::move(stream));
		}
	};
}//namespace loaders
}//namespace graphic
}//namespace pisk

using namespace pisk::tools;

SafeComponentPtr __cdecl shader_loader_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&shader_loader_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto resource_manager = temp_sl.get<pisk::system::ResourceManager>();
	if (resource_manager == nullptr)
		throw pisk::infrastructure::NullPointerException();

	auto resource_loader = factory.make<pisk::graphic::loaders::ShaderLoader>();
	resource_manager->get_loader_registry().register_resource_loader(
		pisk::graphic::ShaderResource::resource_type, resource_loader
	);
	return resource_loader;
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_shader_loader_factory()
{
	static_assert(std::is_convertible<decltype(&get_shader_loader_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &shader_loader_factory;
}

