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

#include <al.h>
#include <alc.h>

namespace pisk
{
namespace audio
{
	inline std::string al_get_error(const ALenum error)
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

