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

#include <pisk/defines.h>
#include <pisk/audio/AudioStream.h>

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

#include <string.h>
#include <functional>

namespace pisk
{
namespace audio
{
namespace loaders
{
	class StreamDecoderBase :
		public PCMAudioStream
	{
		std::size_t channel_count = 0;
		std::size_t sample_size = 0;
		std::size_t bitrate = 0;

	protected:
		void set_traits(const std::size_t _channel_count, const std::size_t _sample_size, const std::size_t _bitrate)
		{
			channel_count = _channel_count;
			sample_size = _sample_size;
			bitrate = _bitrate;
		}

	public:
		virtual std::size_t get_channel_count() const noexcept final override
		{
			return channel_count;
		}

		virtual std::size_t get_sample_size() const noexcept final override
		{
			return sample_size;
		}

		virtual std::size_t get_bitrate() const noexcept final override
		{
			return bitrate;
		}
	};

	using DataBufferPtr = std::shared_ptr<const infrastructure::DataBuffer>;
	using PCMDecoderFactoryFn = std::function<PCMAudioStreamPtr (DataBufferPtr encoded_data)>;
}
}
}

