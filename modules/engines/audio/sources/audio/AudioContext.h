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
			logger::debug("openal", "Audio context going to initialize");
			::alcGetError(device);

			device = ::alcOpenDevice(nullptr);
			if (device == nullptr)
			{
				logger::error("openal", "Unable to open default device: {}", alc_get_error(device));
				throw infrastructure::InitializeError();
			}
			context = ::alcCreateContext(device, nullptr);
			if (context == nullptr)
			{
				logger::error("openal", "Not enouht resource to create new openal context: {}", alc_get_error(device));
				::alcCloseDevice(device);
				throw infrastructure::InitializeError();
			}
			;
			if (::alcMakeContextCurrent(context) == ALC_FALSE)
			{
				logger::error("openal", "Can't change current audio context: {}", alc_get_error(device));
				::alcDestroyContext(context);
				::alcCloseDevice(device);
				throw infrastructure::InitializeError();
			}

			logger::debug("openal", "Audio context initialized");
		}
		~AudioContext()
		{
			logger::debug("openal", "Audio context going to destroy");
			::alcGetError(device);

			if (::alcMakeContextCurrent(nullptr) == ALC_FALSE)
				logger::error("openal", "Can't reset current audio context to default: {}", alc_get_error(device));

			::alcDestroyContext(context);
			if (auto error_code = ::alcGetError(device))
				logger::error("openal", "Can't close audio context: {}", alc_get_error(error_code));

			if (::alcCloseDevice(device) == ALC_FALSE)
				logger::error("openal", "Can't close audio device");

			logger::debug("openal", "Audio context destroied");
		}
	};
}
}

