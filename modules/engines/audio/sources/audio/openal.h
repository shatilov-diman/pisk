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

#include <al.h>
#include <alc.h>

namespace pisk
{
namespace audio
{
	inline std::string al_get_error(const ALCenum error)
	{
		switch (error)
		{
		case AL_NO_ERROR:
			return "No error, " + std::to_string(error);
		case AL_INVALID_NAME:
			return "AL_INVALID_NAME, " + std::to_string(error);
		case AL_INVALID_ENUM:
			return "AL_INVALID_ENUM, " + std::to_string(error);
		case AL_INVALID_VALUE:
			return "AL_INVALID_VALUE, " + std::to_string(error);
		case AL_INVALID_OPERATION:
			return "AL_INVALID_OPERATION, " + std::to_string(error);
		case AL_OUT_OF_MEMORY:
			return "AL_OUT_OF_MEMORY, " + std::to_string(error);
		default:
			return "Unexpected error code, " + std::to_string(error);
		}
	}
	inline std::string alc_get_error(const ALCenum error)
	{
		switch (error)
		{
		case ALC_NO_ERROR:
			return "No error, " + std::to_string(error);
		case ALC_INVALID_DEVICE:
			return "invalid device handle, " + std::to_string(error);
		case ALC_INVALID_CONTEXT:
			return "invalid context handle, " + std::to_string(error);
		case ALC_INVALID_ENUM:
			return "invalid enum parameter passed to an ALC call, " + std::to_string(error);
		case ALC_INVALID_VALUE:
			return "invalid value parameter passed to an ALC call, " + std::to_string(error);
		case ALC_OUT_OF_MEMORY:
			return "out of memory, " + std::to_string(error);

		default:
			return "Unexpected error code, " + std::to_string(error);
		}
	}
	inline std::string alc_get_error(ALCdevice* device)
	{
		return alc_get_error(::alcGetError(device));
	}
}
}

