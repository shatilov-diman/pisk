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

#include <pisk/infrastructure/DataStream.h>

#include <pisk/system/Resource.h>

namespace pisk
{
namespace audio
{
	class PCMAudioStream :
		public system::SubResource
	{
	public:
		virtual infrastructure::DataStream& get_data_stream() noexcept = 0;

		virtual std::size_t get_channel_count() const noexcept = 0;

		virtual std::size_t get_sample_size() const noexcept = 0;

		virtual std::size_t get_bitrate() const noexcept = 0;
	};
	using PCMAudioStreamPtr = system::subresource_ptr<PCMAudioStream>;


	class StreamResource :
		public system::Resource
	{
	public:
		constexpr static const char* resource_type = "sound";

		virtual PCMAudioStreamPtr decode() threadsafe const noexcept = 0;

	};
	using StreamResourcePtr = system::resource_ptr<StreamResource>;
}
}

