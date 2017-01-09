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
#include <pisk/utils/property_tree.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/system/ResourceManager.h>

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
		utils::signaler<utils::keystring> on_stop;

		explicit Engine(const AudioLoaderFn& _audio_loader):
			audio_loader(_audio_loader)
		{
			if (audio_loader == nullptr)
				throw infrastructure::NullPointerException();
		}

		void play(const utils::keystring& id_path, const utils::keystring& res_id)
		{
			infrastructure::Logger::get().debug("audio", "Play %s for %s", res_id.c_str(), id_path.c_str());
			auto&& audio_source = load_audio(res_id);
			if (audio_source != nullptr)
			{
				audio_source->play_once();
				audio_sources[id_path] = std::move(audio_source);
			}
		}
		void stop(const utils::keystring& id_path)
		{
			infrastructure::Logger::get().debug("audio", "Stop for %s", id_path.c_str());
			audio_sources.erase(id_path);
		}
		void stop_all()
		{
			infrastructure::Logger::get().debug("audio", "Stop all");
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
					on_stop.emit(iter.first);
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

