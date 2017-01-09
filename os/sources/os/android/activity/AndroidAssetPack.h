// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module os of the project pisk.
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

#include <pisk/utils/keystring.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/infrastructure/Exception.h>

#include <pisk/system/ResourceManager.h>

#include <pisk/os/android/jni_os_headers.h>

namespace pisk
{
namespace os
{
namespace impl
{

class AndroidAssetsPack : public pisk::system::ResourcePack {
	class FileDataStream : public pisk::infrastructure::DataStream {

		AAsset* asset;

	public:
		static pisk::infrastructure::DataStreamPtr open(AAsset* asset)
		{
			if (asset == nullptr)
				throw infrastructure::NullPointerException();
			return std::unique_ptr<FileDataStream>(new FileDataStream(asset));
		}

		virtual ~FileDataStream()
		{
			AAsset_close(asset);
		}

	private:

		explicit FileDataStream(AAsset* asset) :
			asset(asset)
		{}

		const std::streamsize size() const {
			return AAsset_getLength(asset);
		}

		virtual std::size_t tell() const {
			return seek_for_const(0, Whence::current);
		}

		virtual std::size_t seek(const long pos, const Whence whence) {
			off_t result = 0;
			if (whence == Whence::begin)
				result = AAsset_seek(asset, pos, SEEK_SET);
			else if (whence == Whence::end)
				result = AAsset_seek(asset, pos, SEEK_END);
			else
				result = AAsset_seek(asset, pos, SEEK_CUR);
			if (result < 0)
				return DataStream::error;
			return static_cast<std::size_t>(result);
		}

		virtual std::size_t read(const std::size_t count, pisk::infrastructure::DataBuffer& out) final override {
			out.resize(count);
			const auto result = AAsset_read(asset, out.data(), count);
			if (result < 0)
			{
				out.resize(0);
				return DataStream::error;
			}
			out.resize(static_cast<std::size_t>(result));
			return out.size();
		}

		virtual pisk::infrastructure::DataBuffer readall() const final override {
			const std::size_t pos = tell();
			if (pos == DataStream::error)
				return {};
			const std::size_t size = seek_for_const(0, Whence::end);
			if (size == DataStream::error)
				return {};
			if (seek_for_const(0, Whence::begin) == DataStream::error)
				return {};
			pisk::infrastructure::DataBuffer out;
			if (read_for_const(size, out) == DataStream::error)
				return {};
			seek_for_const(pos, Whence::begin);
			if (tell() != pos)
				throw infrastructure::LogicErrorException();
			return out;
		}

		std::size_t seek_for_const(const long pos, const Whence whence) const {
			return const_cast<FileDataStream*>(this)->seek(pos, whence);
		}
		std::size_t read_for_const(const std::size_t count, pisk::infrastructure::DataBuffer& out) const {
			return const_cast<FileDataStream*>(this)->read(count, out);
		}
	};

	AAssetManager* asset_manager;

public:

	explicit AndroidAssetsPack(AAssetManager* asset_manager):
		asset_manager(asset_manager)
	{
		if (asset_manager == nullptr)
			throw infrastructure::NullPointerException();
	}

	virtual pisk::infrastructure::DataStreamPtr open(const std::string& rid) const noexcept threadsafe final override
	{
		pisk::infrastructure::Logger::get().debug("filepack", "Try to open '%s'", rid.c_str());
		AAsset* asset = AAssetManager_open(asset_manager, rid.c_str(), AASSET_MODE_RANDOM);
		if (asset == nullptr)
			return nullptr;
		return FileDataStream::open(asset);
	}
};

}
}
}

