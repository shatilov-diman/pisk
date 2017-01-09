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


#include <pisk/defines.h>

#include <pisk/infrastructure/Logger.h>

#include "StreamDecoder.h"

#include <algorithm>

namespace pisk
{
namespace audio
{
namespace loaders
{
	class OggStreamDecoder :
		public StreamDecoderBase,
		private infrastructure::DataStream
	{
		DataBufferPtr data;
		std::size_t data_position = 0;
		std::size_t pcm_total_size = 0;

		std::unique_ptr<OggVorbis_File> ogg_file;

	public:
		explicit OggStreamDecoder(const DataBufferPtr& _data) :
			data(_data),
			ogg_file(std::make_unique<OggVorbis_File>())
		{
			if (data == nullptr)
				throw infrastructure::NullPointerException();
			init();
		}
		virtual ~OggStreamDecoder()
		{
			ov_clear(ogg_file.get());
			ogg_file.reset();
		}

		virtual infrastructure::DataStream& get_data_stream() noexcept
		{
			return *this;
		}

	private:
		void init()
		{
			ov_callbacks cb;
			cb.close_func = close_ogg;
			cb.read_func = read_ogg;
			cb.seek_func = seek_ogg;
			cb.tell_func = tell_ogg;

			const auto result = ov_open_callbacks(this, ogg_file.get(), nullptr, -1, cb);
			if (result < 0)
			{
				infrastructure::Logger::get().error("ogg", "Decoding failed with error: %d", result);
				throw infrastructure::InitializeError();
			}

			auto comments = ov_comment(ogg_file.get(), -1);
			UNUSED(comments);
			auto info = ov_info(ogg_file.get(), -1);
			UNUSED(info);
			if (info->channels == 0 or info->channels > 2)
			{
				infrastructure::Logger::get().error("ogg", "Unexpected channels number: %d", info->channels);
				throw infrastructure::InvalidArgumentException();
			}
			if (info->rate < 128 or info->rate > 1024 * 1024 * 1024)
			{
				infrastructure::Logger::get().error("ogg", "Unexpected stream bitrate: %d", info->rate);
				throw infrastructure::InvalidArgumentException();
			}

			set_traits(info->channels, 16 * info->channels, info->rate);

			const ogg_int64_t pcm_total = ov_pcm_total(ogg_file.get(), -1);
			if (pcm_total < 0)
			{
				infrastructure::Logger::get().error("ogg", "Decoding failed with error: %d", result);
				throw infrastructure::InitializeError();
			}
			pcm_total_size = static_cast<std::size_t>(pcm_total);
		}
		std::size_t get_pcm_total_size() const {
			return pcm_total_size;
		}
	private:
		virtual std::size_t tell() const {
			return static_cast<std::size_t>(ov_pcm_tell(ogg_file.get()));
		}
		virtual std::size_t seek(const long pos, const Whence whence)
		{
			int result = 0;
			if (whence == Whence::begin)
				result = ov_pcm_seek(ogg_file.get(), pos);
			else if (whence == Whence::current)
				result = ov_pcm_seek(ogg_file.get(), tell() + pos);
			else if (whence == Whence::end)
			{
				const ogg_int64_t size = ov_pcm_total(ogg_file.get(), -1);
				if (size < 0 or static_cast<std::uint64_t>(size) > std::numeric_limits<std::size_t>::max())
				{
					infrastructure::Logger::get().error("ogg", "Unexpected stream size: %ld", size);
					return std::numeric_limits<std::size_t>::max();
				}
				result = ov_pcm_seek(ogg_file.get(), size + pos);
			}
			else
			{
				infrastructure::Logger::get().error("ogg", "Unexpected seek direction");
				throw infrastructure::InvalidArgumentException();
			}
			if (result < 0)
			{
				infrastructure::Logger::get().error("ogg", "Seeking stream failed with error: %d", result);
				return std::numeric_limits<std::size_t>::max();
			}
			return static_cast<std::size_t>(result);
		}
		virtual std::size_t read(const std::size_t count, pisk::infrastructure::DataBuffer& out) final override
		{
			return read_impl(count, out);
		}
		virtual infrastructure::DataBuffer readall() const
		{
			infrastructure::DataBuffer out;
			read_impl(std::numeric_limits<std::size_t>::max(), out);
			return out;
		}
		std::size_t read_impl(const std::size_t bytes_count, pisk::infrastructure::DataBuffer& out) const
		{
			const int is_big_endian = 0;
			const int is_bytes_per_word = 2;
			const int is_signed = 1;

			int current_section = 0;

			const std::size_t total_size = get_pcm_total_size();
			const std::size_t count = total_size > bytes_count ? bytes_count : total_size;
			out.resize(count);
			std::size_t read_count = 0;
			while (read_count < count)
			{
				const int ret = ov_read(
					ogg_file.get(),
					reinterpret_cast<char*>(out.data()) + read_count,
					count - read_count,
					is_big_endian,
					is_bytes_per_word,
					is_signed,
					&current_section);
				if (ret == 0)
					break;
				if (ret < 0)
				{
					infrastructure::Logger::get().error("ogg", "Reading stream failed with error: %d", ret);
					return std::numeric_limits<std::size_t>::max();
				}
				read_count += ret;
			}
			return read_count;
		}

	private:
		static int close_ogg(void*)
		{
			return 0;
		}
		static std::size_t read_ogg(void* ptr, const std::size_t count, const std::size_t nmemb, void* datasource)
		{
			//TODO:
			//if (count > (std::numeric_limits<std::size_t>::max() >> 8) or nmemb > 256)
			//	throw infrastructure::InvalidArgumentException();

			OggStreamDecoder* decoder = reinterpret_cast<OggStreamDecoder*>(datasource);
			return decoder->read_ogg_impl(count * nmemb, ptr);
		}
		static int seek_ogg(void* datasource, ogg_int64_t pos, int whence)
		{
			if (pos > std::numeric_limits<ogg_int64_t>::max() or pos < std::numeric_limits<ogg_int64_t>::min())
				throw infrastructure::InvalidArgumentException();
			OggStreamDecoder* decoder = reinterpret_cast<OggStreamDecoder*>(datasource);
			return decoder->seek_ogg_impl(pos, whence);
		}
		static long tell_ogg(void* datasource)
		{
			OggStreamDecoder* decoder = reinterpret_cast<OggStreamDecoder*>(datasource);
			return decoder->tell_ogg_impl();
		}

	private:
		std::size_t read_ogg_impl(const std::size_t bytes, void* out_buffer)
		{
			const std::size_t count = std::min(bytes, data->size() - data_position);
			::memcpy(out_buffer, data->data() + data_position, count);
			data_position += count;
			return count;
		}
		int seek_ogg_impl(const ogg_int64_t pos, const int whence)
		{
			switch (whence)
			{
				case SEEK_SET:
					if (pos > static_cast<ogg_int64_t>(data->size()) or pos < 0)
						return -1;
					data_position = static_cast<std::size_t>(pos);
					return 0;
				case SEEK_CUR:
					if (pos > 0)
					{
						const std::size_t cor_pos = static_cast<std::size_t>(pos);
						if (data_position + cor_pos > data->size())
							return -1;
						data_position += cor_pos;
					}
					else
					{
						const std::size_t cor_pos = static_cast<std::size_t>(-pos);
						if (data_position < cor_pos)
							return -1;
						data_position -= cor_pos;
					}
					return 0;
				case SEEK_END:
					if (pos > 0 or static_cast<std::size_t>(-pos) > data->size())
						return -1;
					data_position -= static_cast<std::size_t>(-pos);
					return 0;
			}
			return -1;
		}
		long tell_ogg_impl() const
		{
			return static_cast<long>(data_position);
		}
	};

	PCMAudioStreamPtr make_ogg_decoder(DataBufferPtr encoded_data)
	{
		return std::make_unique<OggStreamDecoder>(encoded_data);
	}
}
}
}

