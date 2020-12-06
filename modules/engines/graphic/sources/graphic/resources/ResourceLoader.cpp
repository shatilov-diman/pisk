
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

#include "../ResourceLoader.h"

#include "ShaderResource.h"

#include "../gl/Shader.h"

#include <deque>
#include <map>

namespace pisk
{
namespace graphic
{
	class GLResourceLoader final :
		public ResourceLoader
	{
		system::ResourceManagerPtr resource_manager;
		std::map<utils::keystring, graphic::subresource_ptr<system::SubResource>> cached_resources;
		std::deque<utils::keystring> cache_free_deque;

		std::map<utils::keystring, graphic::subresource_wptr<system::SubResource>> uses_resources;

	public:
		GLResourceLoader(const system::ResourceManagerPtr& _resource_manager):
			resource_manager(_resource_manager)
		{}

		virtual void release() final override
		{
			delete this;
		}

		virtual ProgramPtr load_program(const res_ids_t& ids) final override
		try
		{
			const auto& super_id = make_super_id("program", ids);
			auto subres = load_subresource_from_cache<Program>(super_id);
			if (subres == nullptr)
			{
				GLShaders shaders;
				for (const auto& pair : ids)
					shaders[pair.first] = load_shader(pair.second, pair.first);
				subres = std::make_shared<Program>(std::move(shaders));
				push_to_cache(super_id, subres);
			}
			return subres;
		}
		catch (const infrastructure::InvalidArgumentException&)
		{
			return nullptr;
		}

		static utils::keystring make_super_id(const utils::keystring& prefix, const res_ids_t& ids)
		{
			utils::keystring out = prefix + ";";
			for (const auto& pair : ids)
				out = out + pair.first + ":" + pair.second;
			return out;
		}

		GLShaderPtr load_shader(const utils::keystring& res_id, const utils::keystring& type)
		{
			return load_resource<GLShader, ShaderSource, ShaderResource>(res_id, type);
		}

	private:
		template <typename GLResourceType, typename InstanceType, typename ResourceInstanceType, typename ... args_t>
		graphic::subresource_ptr<GLResourceType> load_resource(const utils::keystring& res_id, args_t&& ... args)
		try
		{
			auto glres = load_subresource_from_cache<GLResourceType>(res_id);
			if (glres == nullptr)
			{
				auto resource = resource_manager->load<ResourceInstanceType>(res_id.get_content());
				if (resource == nullptr)
					return {};
				auto subres = resource->load(std::forward<args_t>(args)...);
				if (subres == nullptr)
					return {};
				glres = std::make_shared<GLResourceType>(subres);
				push_to_cache(res_id, glres);
			}
			return glres;
		}
		catch (const system::ResourceNotFound&)
		{
			logger::warning("graphic", "resource '{}' not found", res_id);
			return {};
		}
		catch (const system::ResourceFormatNotSupported&)
		{
			logger::warning("graphic", "resource '{}' not supported", res_id);
			return {};
		}

		template <typename GLResourceType>
		graphic::subresource_ptr<GLResourceType> load_subresource_from_cache(const utils::keystring& res_id)
		{
			auto found = cached_resources.find(res_id);
			if (found != cached_resources.end())
				return std::dynamic_pointer_cast<GLResourceType>(found->second);
			auto found2 = uses_resources.find(res_id);
			if (found2 != uses_resources.end())
			{
				auto ptr = found2->second.lock();
				if (ptr != nullptr)
					return std::dynamic_pointer_cast<GLResourceType>(ptr);
				else
					uses_resources.erase(found2);
			}
			for (auto iter = uses_resources.begin(); iter != uses_resources.end();)
				if (iter->second.expired())
					iter = uses_resources.erase(iter);
				else
					++iter;
			return {};
		}

		template <typename GLResourceType>
		void push_to_cache(const utils::keystring& res_id, const graphic::subresource_ptr<GLResourceType>& resource)
		{
			//TODO: replace with more effective cache algorithm:
			//      calculate resource size and compare with full available memory size
			while (cache_free_deque.size() > 32)
			{
				auto& back = cache_free_deque.back();
				cached_resources.erase(back);
				cache_free_deque.pop_back();
			}
			cached_resources[res_id] = resource;
			cache_free_deque.push_front(res_id);
		}
	};

	ResourceLoaderPtr make_resource_loader(const system::ResourceManagerPtr& resource_manager)
	{
		return tools::make_unique_releasable<GLResourceLoader>(resource_manager);
	}
}//namespace graphic
}//namespace pisk

