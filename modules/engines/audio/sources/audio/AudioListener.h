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
	class AudioListener
	{
	public:
		AudioListener()
		{
			::alListener3f(AL_POSITION, 0.f, 0.f, 1.f);
			::alListener3f(AL_VELOCITY, 0.f, 0.f, 1.f);

			ALfloat listenerOri[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 0.f };
			::alListenerfv(AL_ORIENTATION, listenerOri);

			if (auto error_code = ::alGetError())
				logger::error("openal", "Listener: {}", al_get_error(error_code));
		}
	};
}
}

