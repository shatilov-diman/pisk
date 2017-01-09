// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module script of the project pisk.
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
#include <pisk/utils/algorithm_utils.h>
#include <pisk/utils/json_utils.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/system/EngineStrategy.h>

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

	private:

		virtual void patch_scene(const system::PatchPtr& patch) final override
		{
			model::ConstReflectedScene scene_object(scene, *patch);
			walk(scene_object, "");
			scene = utils::property::merge(scene, *patch);

			execute("script", "patch_scene", {*patch});
		}

		virtual void update() final override
		{
			execute("script", "update", {});
		}

	private:
		void walk(model::ConstReflectedObject& object, const utils::keystring& id_path)
		try
		{
			const utils::keystring& obj_id = object.id().is_string() ? object.id().as_keystring() : "";
			const utils::keystring& obj_id_path = model::path::add(id_path, obj_id);
			process_object(object, obj_id_path);
			if (object.children().has_changes())
				for (auto child : object.children())
					if (child.has_changes())
						walk(child, obj_id_path);
		}
		catch (const model::UnexpectedItemTypeException&)
		{
			infrastructure::Logger::get().warning("script", "Unexpected item type");
		}

		void process_object(model::ConstReflectedObject& object, const utils::keystring& id_path)
		{
			auto presentation = object.presentation<model::script::ConstPresentation>();
			if (presentation.is_none())
			{
				infrastructure::Logger::get().debug("script", "Object '%s' does not contains script presentation", id_path.c_str());
				return;
			}
			process_presentation(object, presentation, id_path);
		}
		void process_presentation(
			model::ConstReflectedObject& object,
			model::script::ConstPresentation& presentation,
			const utils::keystring& id_path
		)
		{
			const auto& state_id = object.current_state_id();
			if (object.has_origin())
				infrastructure::Logger::get().debug("script", "Receive new state for object '%s': '%s'", id_path.c_str(), state_id.as_keystring().c_str());
			else
				infrastructure::Logger::get().debug("script", "Receive new object '%s' with initial state: '%s'", id_path.c_str(), state_id.as_keystring().c_str());

			const auto& resource_id = presentation.state(state_id.as_keystring()).res_id();
			const auto& function = presentation.state(state_id.as_keystring()).function();
			const auto& arguments = presentation.state(state_id.as_keystring()).arguments();
			if (not (resource_id.is_string() and function.is_string() and arguments.is_array()))
			{
				infrastructure::Logger::get().warning("script", "State '%s' has not requred params or them has incorrect types (object: '%s')", state_id.as_keystring().c_str(), id_path.c_str());
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
			infrastructure::Logger::get().critical("script", "to_property: unexpected item type");
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

