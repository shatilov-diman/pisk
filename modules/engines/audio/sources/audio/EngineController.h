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

#include <pisk/utils/signaler.h>

#include <pisk/model/Path.h>
#include <pisk/model/ReflectedScene.h>
#include <pisk/model/audio/ReflectedPresentation.h>

#include "Engine.h"

namespace pisk
{
namespace audio
{
	class EngineController
	{
		utils::property scene;
		Engine audio_engine;

	public:
		utils::signaler<utils::keystring> on_start_play;
		utils::signaler<utils::keystring> on_finish_play;

		explicit EngineController(const AudioLoaderFn& audio_loader):
			audio_engine(audio_loader)
		{
			audio_engine.on_start_play += this->on_start_play;
			audio_engine.on_finish_play += this->on_finish_play;
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
			walk(scene_object, {});
			utils::property::replace(scene, *patch);
		}

		void update()
		{
			audio_engine.update();
		}

	private:
		void clear_model()
		{
			audio_engine.stop_all();
		}

		void walk(model::ConstReflectedObject& object, const model::PathId& id_path)
		try
		{
			const utils::keystring& obj_id = object.id().is_string() ? object.id().as_keystring() : "";
			const auto& obj_id_path = id_path.add(obj_id);
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
			logger::warning("audio", "Unexpected item type");
		}

		void process_object(model::ConstReflectedObject& object, const model::PathId& id_path)
		{
			auto presentation = object.presentation<model::audio::ConstPresentation>();
			if (presentation.is_none())
			{
				logger::spam("audio", "Object '{}' does not contains audio presentation", id_path);
				return;
			}
			process_presentation(object, presentation, id_path);
		}
		void process_presentation(
			model::ConstReflectedObject& object,
			model::audio::ConstPresentation& presentation,
			const model::PathId& id_path
		)
		{
			const auto& state_id = object.current_state_id();
			if (object.has_origin())
				logger::debug("audio", "Receive new state for object '{}': '{}'", id_path, state_id.as_keystring());
			else
				logger::debug("audio", "Receive new object '{}' with initial state: '{}'", id_path, state_id.as_keystring());

			const auto& resource_id = presentation.state(state_id.as_keystring()).res_id();
			if (resource_id.is_none())
			{
				logger::debug("audio", "State '{}' does not contains res_id (object: '{}')", state_id.as_keystring(), id_path);
				audio_engine.stop(id_path);
				return;
			}
			audio_engine.play(id_path, resource_id.as_keystring());
		}
		void on_remove(const model::PathId& id_path)
		{
			audio_engine.stop(id_path);
		}
	};
}
}

