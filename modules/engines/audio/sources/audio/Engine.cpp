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


#include <pisk/defines.h>

#include <pisk/tools/ComponentsLoader.h>
#include <pisk/system/EngineComponentFactory.h>

#include "Engine.h"
#include "EngineStrategy.h"

using namespace pisk;
using namespace pisk::audio;

const utils::keystring EngineController::k_inactive = "inactive";
const utils::keystring EngineController::k_children = "children";
const utils::keystring EngineController::k_presentations = "presentations";

const utils::keystring EngineController::k_properties = "properties";
const utils::keystring EngineController::k_object_id = "id";
const utils::keystring EngineController::k_active_state = "state";

const utils::keystring EngineController::k_states = "states";

const utils::keystring EngineController::k_engine_id = "audio";
const utils::keystring EngineController::k_res_id = "res_id";

using namespace pisk::tools;

SafeComponentPtr __cdecl audio_engine_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&audio_engine_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto engine_factory = temp_sl.get<pisk::system::EngineComponentFactory>();
	auto res_manager = temp_sl.get<pisk::system::ResourceManager>();
	if (engine_factory == nullptr or res_manager == nullptr)
		return {};

	auto audio_loader = [res_manager](const utils::keystring& res_id) -> AudioSourcePtr {
		try {
			StreamResourcePtr&& stream_resource = res_manager->load<StreamResource>(res_id.get_content());
			return std::make_unique<AudioSource>(std::move(stream_resource));
		} catch (infrastructure::Exception&) {
			infrastructure::Logger::get().error("openal", "Load audio failed");
			return nullptr;
		}
	};

	return engine_factory->make_engine(
		factory,
		[audio_loader](pisk::system::PatchRecipient& patch_recipient) {
			return std::make_unique<pisk::audio::EngineStrategy>(audio_loader, patch_recipient);
		}
	);
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_audio_engine_factory()
{
	return &audio_engine_factory;
}

