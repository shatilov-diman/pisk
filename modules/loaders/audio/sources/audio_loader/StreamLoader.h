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

#include "StreamResource.h"

#include <pisk/system/ResourceLoader.h>

namespace pisk
{
namespace audio
{
namespace loaders
{
	class StreamLoader :
		public system::ResourceLoader
	{
		const infrastructure::DataBuffer signature;
		PCMDecoderFactoryFn decoder_factory;

		virtual void release() final override
		{
			delete this;
		}

		virtual bool can_load(const infrastructure::DataStreamPtr& stream) const noexcept threadsafe final override
		{
			infrastructure::DataBuffer stream_signature;

			const auto orig_pos = stream->tell();
			stream->seek(0, infrastructure::DataStream::Whence::begin);
			stream->read(signature.size(), stream_signature);
			stream->seek(orig_pos, infrastructure::DataStream::Whence::begin);

			return signature == stream_signature;
		}

		virtual system::ResourcePtr load(infrastructure::DataStreamPtr&& stream) const noexcept threadsafe final override
		{
			if (stream == nullptr)
				return nullptr;
			return std::make_shared<StreamResourceImpl>(decoder_factory, std::move(stream));
		}
	public:
		explicit StreamLoader(const infrastructure::DataBuffer& signature, PCMDecoderFactoryFn _decoder_factory) :
			signature(signature),
			decoder_factory(_decoder_factory)
		{}
	};
}
}
}

