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

