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
			engine.on_stop += [this](const utils::keystring& id_path) {
				this->push_stop_event(id_path);
			};
		}

	private:
		void push_stop_event(const utils::keystring& id_path)
		{
			system::Patch patch;
			model::ReflectedEvent event(utils::property::none_property(), patch);
			event.source() = "audio";
			event.type() = "control";
			event.action() = "stop";
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
			pisk::infrastructure::Logger::get().debug("quest", "Update scene:\n %s", content.c_str());

			push_changes(patch);
		}
	};
}
}

