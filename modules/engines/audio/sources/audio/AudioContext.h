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

#include <pisk/infrastructure/Logger.h>

#include "openal.h"

namespace pisk
{
namespace audio
{
	class AudioContext
	{
		ALCdevice* device;
		ALCcontext* context;
	public:
		AudioContext():
			device(nullptr),
			context(nullptr)
		{
			infrastructure::Logger::get().debug("openal", "Audio context going to initialize");
			::alcGetError(device);

			device = ::alcOpenDevice(nullptr);
			if (device == nullptr)
			{
				infrastructure::Logger::get().error("openal", "Unable to open default device: %s", alc_get_error(device).c_str());
				throw infrastructure::InitializeError();
			}
			context = ::alcCreateContext(device, nullptr);
			if (context == nullptr)
			{
				infrastructure::Logger::get().error("openal", "Not enouht resource to create new openal context: %s", alc_get_error(device).c_str());
				::alcCloseDevice(device);
				throw infrastructure::InitializeError();
			}
			;
			if (::alcMakeContextCurrent(context) == ALC_FALSE)
			{
				infrastructure::Logger::get().error("openal", "Can't change current audio context: %s", alc_get_error(device).c_str());
				::alcDestroyContext(context);
				::alcCloseDevice(device);
				throw infrastructure::InitializeError();
			}

			infrastructure::Logger::get().debug("openal", "Audio context initialized");
		}
		~AudioContext()
		{
			infrastructure::Logger::get().debug("openal", "Audio context going to destroy");
			::alcGetError(device);

			if (::alcMakeContextCurrent(nullptr) == ALC_FALSE)
				infrastructure::Logger::get().error("openal", "Can't reset current audio context to default: %s", alc_get_error(device).c_str());

			::alcDestroyContext(context);
			if (auto error_code = ::alcGetError(device))
				infrastructure::Logger::get().error("openal", "Can't close audio context: %s", alc_get_error(error_code).c_str());

			if (::alcCloseDevice(device) == ALC_FALSE)
				infrastructure::Logger::get().error("openal", "Can't close audio device");

			infrastructure::Logger::get().debug("openal", "Audio context destroied");
		}
	};
}
}

