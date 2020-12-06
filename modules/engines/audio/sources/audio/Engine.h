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
#include <pisk/utils/property_tree.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/system/ResourceManager.h>

#include <pisk/model/Path.h>

#include <memory>
#include <string>

#include "AudioContext.h"
#include "AudioSource.h"
#include "AudioListener.h"

namespace pisk
{
namespace audio
{
	using AudioLoaderFn = std::function<AudioSourcePtr (const utils::keystring& res_id)>;

	class Engine
	{
		AudioLoaderFn audio_loader;

		AudioContext context;
		AudioListener listener;
		std::map<utils::keystring, AudioSourcePtr> audio_sources;
	public:
		utils::signaler<utils::keystring> on_start_play;
		utils::signaler<utils::keystring> on_finish_play;

		explicit Engine(const AudioLoaderFn& _audio_loader):
			audio_loader(_audio_loader)
		{
			if (audio_loader == nullptr)
				throw infrastructure::NullPointerException();
		}

		void play(const model::PathId& id_path, const utils::keystring& res_id)
		{
			logger::debug("audio", "Play {} for {}", res_id, id_path);
			auto&& audio_source = load_audio(res_id);
			if (audio_source != nullptr)
			{
				auto&& id_path_str = id_path.to_keystring();
				audio_source->play_once();
				audio_sources[id_path_str] = std::move(audio_source);
				on_start_play.emit(id_path_str);
			}
		}
		void stop(const model::PathId& id_path)
		{
			logger::debug("audio", "Stop for {}", id_path);
			audio_sources.erase(id_path.to_keystring());
		}
		void stop_all()
		{
			logger::debug("audio", "Stop all");
			audio_sources.clear();
		}

		void update()
		{
			std::deque<AudioSourcePtr> trush;
			for (auto& iter : audio_sources)
			{
				if (iter.second == nullptr)
					continue;
				iter.second->update();
				if (iter.second->is_stopped())
				{
					trush.emplace_back(std::move(iter.second));
					on_finish_play.emit(iter.first);
				}
			}
			for (auto iter = audio_sources.begin(); iter != audio_sources.end();)
				if (iter->second == nullptr)
					iter = audio_sources.erase(iter);
				else
					++iter;
		}

	private:
		AudioSourcePtr load_audio(const utils::keystring& res_id) const
		{
			return audio_loader(res_id);
		}
	};

}
}

