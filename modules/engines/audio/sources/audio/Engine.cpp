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


#include <pisk/defines.h>

#include <pisk/tools/ComponentsLoader.h>
#include <pisk/system/EngineComponentFactory.h>

#include "Engine.h"
#include "EngineStrategy.h"

using namespace pisk;
using namespace pisk::audio;
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
		} catch (system::ResourceNotFound&) {
			logger::error("openal", "Audio resource '{}' not found", res_id);
			return nullptr;
		} catch (infrastructure::Exception&) {
			logger::error("openal", "Loading of audio resource '{}' was failed", res_id);
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

