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
#include <pisk/utils/algorithm_utils.h>
#include <pisk/utils/json_utils.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/system/EngineStrategy.h>

#include <pisk/model/Path.h>
#include <pisk/model/ReflectedScene.h>
#include <pisk/model/script/ReflectedPresentation.h>

#include "ScriptManager.h"


namespace pisk
{
namespace script
{
	class EngineStrategy :
		public system::EngineStrategyBase
	{
		ScriptManager script_manager;
		pisk::utils::property config;
		utils::property scene;

 	public:
		EngineStrategy(const system::ResourceManagerPtr& resource_manager, system::PatchRecipient& patch_recipient, const pisk::utils::property& config):
			system::EngineStrategyBase(patch_recipient),
			script_manager(resource_manager),
			config(config)
		{}

		virtual Configure on_init_app() override
		{
			execute("script", "init", {config});
			return {};
		}

		virtual void on_deinit_app() override
		{
			execute("script", "deinit", {});
		}

		virtual void prepatch() final override
		{
			execute("script", "prepatch", {});
		}

		virtual void patch_scene(const system::PatchPtr& patch) final override
		{
			model::ConstReflectedScene scene_object(scene, *patch);
			walk(scene_object, {});
			utils::property::replace(scene, *patch);

			execute("script", "patch_scene", {*patch});
		}

		virtual void update() final override
		{
			execute("script", "update", {});
		}

	private:
		void walk(model::ConstReflectedObject& object, const model::PathId& id_path)
		try
		{
			const utils::keystring& obj_id = object.id().is_string() ? object.id().as_keystring() : "";
			const auto& obj_id_path = id_path.add(obj_id);
			process_object(object, obj_id_path);
			if (object.children().has_changes())
				for (auto child : object.children())
					if (child.has_changes())
						walk(child, obj_id_path);
		}
		catch (const model::UnexpectedItemTypeException&)
		{
			logger::warning("script", "Unexpected item type");
		}

		void process_object(model::ConstReflectedObject& object, const model::PathId& id_path)
		{
			auto presentation = object.presentation<model::script::ConstPresentation>();
			if (presentation.is_none())
			{
				logger::spam("script", "Object '{}' does not contains script presentation", id_path);
				return;
			}
			process_presentation(object, presentation, id_path);
		}
		void process_presentation(
			model::ConstReflectedObject& object,
			model::script::ConstPresentation& presentation,
			const model::PathId& id_path
		)
		{
			const auto& state_id = object.current_state_id();
			if (object.has_origin())
				logger::debug("script", "Receive new state for object '{}': '{}'", id_path, state_id.as_keystring());
			else
				logger::debug("script", "Receive new object '{}' with initial state: '{}'", id_path, state_id.as_keystring());

			const auto& resource_id = presentation.state(state_id.as_keystring()).res_id();
			const auto& function = presentation.state(state_id.as_keystring()).function();
			const auto& arguments = presentation.state(state_id.as_keystring()).arguments();
			if (not (resource_id.is_string() and function.is_string() and arguments.is_array()))
			{
				logger::warning("script", "State '{}' has not requred params or them has incorrect types (object: '{}')", state_id.as_keystring(), id_path);
				return;
			}
			execute(resource_id.as_keystring(), function.as_keystring(), to_arguments(arguments));
		}

	protected:
		static Arguments to_arguments(const model::ConstReflectedItem& item)
		{
			if (item.is_none())
				return {};
			if (item.is_bool())
				return { item.as_bool() };
			if (item.is_int())
				return { item.as_int() };
			if (item.is_string())
				return { item.as_keystring() };
			if (item.is_array())
			{
				const std::size_t count = item.size();
				Arguments out;
				out.resize(count);
				for (std::size_t index = 0; index < count; ++index)
				{
					auto&& args = to_arguments(item.get_item(index));
					for (auto& arg : args)
						out[index] = std::move(arg);
				}
				return out;
			}
			if (item.is_dictionary())
			{
				utils::property out;
				for (const auto& key : item.get_members())
				{
					auto&& args = to_arguments(item.get_item(key));
					for (const auto& arg : args)
						out[key] = std::move(arg);
				}
				return { out };
			}
			logger::critical("script", "to_property: unexpected item type");
			throw infrastructure::LogicErrorException();
		}

		void execute(const utils::keystring& resource_id, const utils::keystring& function, const Arguments& arguments)
		{
			get_script_manager().execute(resource_id, function, arguments);
		}

		ScriptManager& get_script_manager()
		{
			return script_manager;
		}
	};

}
}

