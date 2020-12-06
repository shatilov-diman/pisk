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

#include <pisk/utils/json_utils.h>

#include <pisk/system/EngineStrategy.h>
#include <pisk/system/ResourceManager.h>

#include "EngineController.h"

namespace pisk
{
namespace audio
{
	class EngineStrategy :
		public system::EngineStrategyBase
	{
		EngineController engine;

		virtual Configure on_init_app() final override
		{
			return {};
		}

		virtual void on_deinit_app() final override
		{}

		virtual void prepatch() final override
		{}

		virtual void patch_scene(const pisk::system::PatchPtr& scene) final override
		{
			engine.apply_changes(scene);
		}

		virtual void update() final override
		{
			engine.update();
		}

	public:
		EngineStrategy(const AudioLoaderFn& audio_loader, system::PatchRecipient& patch_recipient):
			system::EngineStrategyBase(patch_recipient),
			engine(audio_loader)
		{
			engine.on_start_play += [this](const utils::keystring& id_path) {
				this->push_start_play_event(id_path);
			};
			engine.on_finish_play += [this](const utils::keystring& id_path) {
				this->push_finish_play_event(id_path);
			};
		}

	private:
		void push_start_play_event(const utils::keystring& id_path)
		{
			push_control_play_event(id_path, "start");
		}
		void push_finish_play_event(const utils::keystring& id_path)
		{
			push_control_play_event(id_path, "finished");
		}
		void push_control_play_event(const utils::keystring& id_path, const utils::keystring& action)
		{
			system::Patch patch;
			model::ReflectedEvent event(utils::property::none_property(), patch);
			event.source() = "audio";
			event.type() = "control";
			event.action() = action;
			event.get_string_item("id_path") = id_path;

			system::Patch out;
			out["events"][std::size_t(0)] = patch;
			push(std::move(out));
		}
		void push(pisk::system::Patch&& patch) noexcept threadsafe
		{
			push(std::make_shared<pisk::system::Patch>(std::move(patch)));
		}
		void push(const pisk::system::PatchPtr& patch) noexcept threadsafe
		{
			const auto& content = pisk::utils::json::to_string(*patch);
			pisk::logger::debug("audio", "Update scene:\n {}", content);

			push_changes(patch);
		}
	};
}
}

