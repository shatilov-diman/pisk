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

#include <pisk/infrastructure/Logger.h>

#include <pisk/system/PropertyLoader.h>

namespace pisk
{
namespace system
{
	template <typename BaseStrategy>
	class EngineStrategyBL :
		public BaseStrategy
	{
		system::ResourceManagerPtr resource_manager;

	public:
		template <typename ... TArgs>
		EngineStrategyBL(const system::ResourceManagerPtr& _resource_manager, TArgs& ... args):
			BaseStrategy(_resource_manager, args...),
			resource_manager(_resource_manager)
		{
			if (resource_manager == nullptr)
				throw infrastructure::InvalidArgumentException();
		}

		bool load_scene(const pisk::utils::keystring& scene_rid, const bool reset_current_scene)
		try
		{
			pisk::logger::debug("script", "Parsing '{}' scene", scene_rid);
			auto&& prop = load_property_tree(scene_rid);

			if (reset_current_scene)
			{
				pisk::logger::debug("script", "Unload current scene");
				auto reset_patch = std::make_shared<Patch>(std::move(prop));
				this->patch_scene(reset_patch);
				this->push_changes(reset_patch);
			}

			pisk::logger::debug("script", "Load '{}' scene", scene_rid);
			auto patch = std::make_shared<Patch>(std::move(prop));
			this->patch_scene(patch);
			this->push_changes(patch);

			return true;
		}
		catch (const system::ResourceNotFound&)
		{
			pisk::logger::warning("script", "Scene '{}' not found", scene_rid);
			return false;
		}
		catch (const infrastructure::Exception& ex)
		{
			logger::warning("lua", "Unexpected exception received while load scene '{}': {}", scene_rid, typeid(ex).name());
			return false;
		}
		catch (const std::exception& ex)
		{
			logger::warning("lua", "Unexpected std::exception received while load scene '{}': {}", scene_rid, ex.what());
			throw;
		}

		pisk::utils::property load_property_tree(const pisk::utils::keystring& res_id)
		{
			auto&& resource = resource_manager->load<pisk::system::PropertyTreeResource>(res_id.get_content());
			return resource->get();
		}
	};
}
}

