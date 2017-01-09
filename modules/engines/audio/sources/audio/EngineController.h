// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module audio of the project pisk.
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

#include <pisk/utils/signaler.h>

#include <pisk/model/ReflectedScene.h>
#include <pisk/model/audio/ReflectedPresentation.h>

#include "Engine.h"

namespace pisk
{
namespace audio
{
	class EngineController
	{
		static const utils::keystring k_inactive;
		static const utils::keystring k_children;
		static const utils::keystring k_presentations;

		static const utils::keystring k_properties;
		static const utils::keystring k_object_id;
		static const utils::keystring k_active_state;

		static const utils::keystring k_states;

		static const utils::keystring k_engine_id;
		static const utils::keystring k_res_id;

		utils::property scene;
		Engine audio_engine;
	public:
		utils::signaler<utils::keystring> on_stop;

		explicit EngineController(const AudioLoaderFn& audio_loader):
			audio_engine(audio_loader)
		{
			audio_engine.on_stop += this->on_stop;
		}
		~EngineController()
		{}

		void apply_changes(const pisk::system::PatchPtr& patch)
		{
			if (patch == nullptr)
				throw infrastructure::NullPointerException();

			if (patch->is_none())
				return clear_model();
			if (not patch->is_dictionary())
				throw pisk::infrastructure::LogicErrorException();

			model::ConstReflectedScene scene_object(scene, *patch);
			walk(scene_object, "");
			scene = utils::property::merge(scene, *patch);
		}

		void update()
		{
			audio_engine.update();
		}

		system::PatchPtr get_changes()
		{
			return {};
		}

	private:
		void clear_model()
		{
			audio_engine.stop_all();
		}

		void walk(model::ConstReflectedObject& object, const utils::keystring& id_path)
		try
		{
			const utils::keystring& obj_id = object.id().is_string() ? object.id().as_keystring() : "";
			const utils::keystring& obj_id_path = model::path::add(id_path, obj_id);
			process_object(object, obj_id_path);
			if (object.children().has_changes())
				for (auto child : object.children())
				{
					if (child.has_changes())
						walk(child, obj_id_path);
					else
						on_remove(obj_id_path);
				}
		}
		catch (const model::UnexpectedItemTypeException&)
		{
			infrastructure::Logger::get().warning("audio", "Unexpected item type");
		}

		void process_object(model::ConstReflectedObject& object, const utils::keystring& id_path)
		{
			auto presentation = object.presentation<model::audio::ConstPresentation>();
			if (presentation.is_none())
			{
				infrastructure::Logger::get().debug("audio", "Object '%s' does not contains audio presentation", id_path.c_str());
				return;
			}
			process_presentation(object, presentation, id_path);
		}
		void process_presentation(
			model::ConstReflectedObject& object,
			model::audio::ConstPresentation& presentation,
			const utils::keystring& id_path
		)
		{
			const auto& state_id = object.current_state_id();
			if (object.has_origin())
				infrastructure::Logger::get().debug("audio", "Receive new state for object '%s': '%s'", id_path.c_str(), state_id.as_keystring().c_str());
			else
				infrastructure::Logger::get().debug("audio", "Receive new object '%s' with initial state: '%s'", id_path.c_str(), state_id.as_keystring().c_str());

			const auto& resource_id = presentation.state(state_id.as_keystring()).res_id();
			if (resource_id.is_none())
			{
				infrastructure::Logger::get().debug("audio", "State '%s' does not contains res_id (object: '%s')", state_id.as_keystring().c_str(), id_path.c_str());
				audio_engine.stop(id_path);
				return;
			}
			audio_engine.play(id_path, resource_id.as_keystring());
		}
		void on_remove(const utils::keystring& id_path)
		{
			audio_engine.stop(id_path);
		}
	};
}
}

