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
				infrastructure::Logger::get().error("openal", "Listener: %s", al_get_error(error_code).c_str());
		}
	};
}
}

