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

#include "StreamDecoder.h"

#include <pisk/audio/AudioStream.h>

namespace pisk
{
namespace audio
{
namespace loaders
{
	class StreamResourceImpl :
		public StreamResource
	{
		PCMDecoderFactoryFn decoder_factory;
		DataBufferPtr data;

	public:
		explicit StreamResourceImpl(PCMDecoderFactoryFn _decoder_factory, infrastructure::DataStreamPtr&& raw_stream) :
			decoder_factory(_decoder_factory)
		{
			if (decoder_factory == nullptr or raw_stream == nullptr)
				throw infrastructure::NullPointerException();
			data = std::make_shared<infrastructure::DataBuffer>(raw_stream->readall());
		}

	private:
		virtual PCMAudioStreamPtr decode() threadsafe const noexcept final override
		{
			return decoder_factory(data);
		}

	};
}
}
}

