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
#include <pisk/audio/AudioStream.h>

#include "openal.h"

namespace pisk
{
namespace audio
{
	class Buffer
	{
		ALuint buffer_id;
	public:
		Buffer():
			buffer_id(0)
		{
			::alGenBuffers(1, &buffer_id);
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Unable to allocate audio buffer: %s", al_get_error(error_code).c_str());
		}
		~Buffer()
		{
			if (buffer_id != 0)
			{
				::alDeleteBuffers(1, &buffer_id);
				if (auto error_code = ::alGetError())
					infrastructure::Logger::get().error("openal", "Unable to release audio buffer: %s", al_get_error(error_code).c_str());
			}
		}
		ALuint handle() const
		{
			return buffer_id;
		}
	};
	class Source
	{
		ALuint source_id;
	public:
		Source():
			source_id(0)
		{
			::alGenSources(1, &source_id);
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Unable to allocate audio source: %s", al_get_error(error_code).c_str());
		}
		~Source()
		{
			if (source_id != 0)
			{
				::alDeleteSources(1, &source_id);
				if (auto error_code = ::alGetError())
					infrastructure::Logger::get().error("openal", "Unable to release audio source: %s", al_get_error(error_code).c_str());
			}
		}
		ALuint handle() const
		{
			return source_id;
		}
	};
	class AudioSource
	{
		PCMAudioStreamPtr data_stream;
		infrastructure::DataBuffer data_buffer;

		ALenum audio_format;
		ALuint audio_freq;
		std::size_t block_size;

		ALint current_state;

		bool looped;//We should manage looping of streamed source externally
		bool data_ends;

		Source source;
		Buffer buffers[2];
	public:
		explicit AudioSource(StreamResourcePtr&& stream):
			audio_format(0),
			audio_freq(0),
			block_size(0),
			current_state(AL_STOPPED),
			looped(false),
			data_ends(false)
		{
			if (stream == nullptr)
			{
				infrastructure::Logger::get().critical("openal", "Unable to build audio source from nullptr");
				throw infrastructure::NullPointerException();
			}
			::alSourcef(source.handle(), AL_PITCH, 1.f);
			::alSourcef(source.handle(), AL_GAIN, .1f);
			::alSourcei(source.handle(), AL_LOOPING, AL_FALSE);
			::alSource3f(source.handle(), AL_POSITION, 0.f, 0.f, 0.f);
			::alSource3f(source.handle(), AL_VELOCITY, 0.f, 0.f, 0.f);
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Source: %s", al_get_error(error_code).c_str());

			data_stream = stream->decode();
			if (data_stream == nullptr)
			{
				infrastructure::Logger::get().critical("openal", "Unable to decode audio source");
				throw infrastructure::NullPointerException();
			}
			load_metainfo();
		}
		~AudioSource()
		{
			stop();

		}
		void play_once()
		{
			play(false);
		}
		void play_looped()
		{
			play(true);
		}

	private:
		void play(bool _looped)
		{
			stop();

			looped = _looped;
			reset_data_ends_flag();
			set_state(AL_PLAYING);

			data_stream->get_data_stream().seek(0, infrastructure::DataStream::Whence::begin);
			fill_buffer(buffers[0].handle());
			fill_buffer(buffers[1].handle());

			ALuint handles[] = {buffers[0].handle(), buffers[1].handle()};
			::alSourceQueueBuffers(source.handle(), countof(handles), handles);
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Source: %s", al_get_error(error_code).c_str());

			::alSourcePlay(source.handle());
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Source::play: %s", al_get_error(error_code).c_str());
		}

	public:
		void stop()
		{
			::alSourceStop(source.handle());
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Source::stop: %s", al_get_error(error_code).c_str());
			set_state(AL_STOPPED);

			ALint queued;
			::alGetSourcei(source.handle(), AL_BUFFERS_QUEUED, &queued);
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "~AudioSource: %s", al_get_error(error_code).c_str());

			while (queued--)
			{
				ALuint buffer_id;
				::alSourceUnqueueBuffers(source.handle(), 1, &buffer_id);
				if (auto error_code = ::alGetError())
					infrastructure::Logger::get().error("openal", "~AudioSource: %s", al_get_error(error_code).c_str());
			}
		}
		void update()
		{
			if (load_buffers())
				check_and_force_state();
			else
				set_state(AL_STOPPED);
		}
		bool is_playing() const
		{
			return is_current_state(AL_PLAYING);
		}
		bool is_stopped() const
		{
			return is_current_state(AL_STOPPED);
		}
		bool is_looped() const
		{
			return looped;
		}

	private:
		void check_and_force_state() const
		{
			const ALint actual_state = get_actual_state();
			if (is_current_state(actual_state))
				return;
			if (is_playing())
				::alSourcePlay(source.handle());
			else
				::alSourceStop(source.handle());
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Source::check_and_force_state: %s", al_get_error(error_code).c_str());
		}

	private:
		void set_state(const ALenum state)
		{
			current_state = state;
		}
		bool is_current_state(const ALenum state) const
		{
			return current_state == state;
		}
		void reset_data_ends_flag()
		{
			data_ends = false;
		}
		void set_data_ends_flag()
		{
			data_ends = true;
		}
		bool is_data_ends() const
		{
			return data_ends;
		}

	private:
		ALint get_actual_state() const
		{
			ALint state = AL_STOPPED;
			::alGetSourcei(source.handle(), AL_SOURCE_STATE, &state);
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Source::get_state: %s", al_get_error(error_code).c_str());
			return state;
		}
		bool load_buffers()
		{
			ALint processed;
			::alGetSourcei(source.handle(), AL_BUFFERS_PROCESSED, &processed);
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Source::load_buffers: %s", al_get_error(error_code).c_str());

			while (processed--)
			{
				ALuint buffer_id;
				::alSourceUnqueueBuffers(source.handle(), 1, &buffer_id);
				if (auto error_code = ::alGetError())
					infrastructure::Logger::get().error("openal", "Source::load_buffers: %s", al_get_error(error_code).c_str());
				if (is_data_ends())
					continue;
				if (not fill_buffer(buffer_id))
				{
					set_data_ends_flag();
					continue;
				}
				::alSourceQueueBuffers(source.handle(), 1, &buffer_id);
				if (auto error_code = ::alGetError())
					infrastructure::Logger::get().error("openal", "Source::load_buffers: %s", al_get_error(error_code).c_str());
			}
			return not is_data_ends();
		}

	private:
		void load_metainfo()
		{
			const auto channels = data_stream->get_channel_count();
			const auto sample_size = data_stream->get_sample_size();

			audio_format = get_al_format(channels, sample_size);
			audio_freq = static_cast<ALuint>(data_stream->get_bitrate());
			block_size = audio_freq * sample_size;
		}
		ALenum get_al_format(const std::size_t channels, const std::size_t sample_size)
		{
			if (channels == 1)
			{
				if (sample_size == 8)
					return AL_FORMAT_MONO8;
				else if (sample_size == 16)
					return AL_FORMAT_MONO16;
			}
			if (channels == 2)
			{
				if (sample_size == 16)
					return AL_FORMAT_STEREO8;
				else if(sample_size == 32)
					return AL_FORMAT_STEREO16;
			}
			throw infrastructure::InvalidArgumentException();
		}

		bool fill_buffer(const ALuint buffer_id)
		{
			std::size_t ret = data_stream->get_data_stream().read(block_size, data_buffer);
			if (ret == 0)
			{
				data_stream->get_data_stream().seek(0, infrastructure::DataStream::Whence::begin);
				if (not is_looped())
					return false;
				ret = data_stream->get_data_stream().read(block_size, data_buffer);
			}
			if (ret == 0 or ret > block_size)
				throw infrastructure::InvalidArgumentException();

			::alBufferData(buffer_id, audio_format, data_buffer.data(), ret, audio_freq);
			if (auto error_code = ::alGetError())
				infrastructure::Logger::get().error("openal", "Source::fill_buffer: %s", al_get_error(error_code).c_str());
			return true;
		}
	};
	using AudioSourcePtr = std::unique_ptr<AudioSource>;
}
}

