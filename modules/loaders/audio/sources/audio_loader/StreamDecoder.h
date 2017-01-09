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

