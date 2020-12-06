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

#include <pisk/utils/keystring.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/infrastructure/Exception.h>

#include <pisk/system/ResourceManager.h>

#include <fstream>

class FileResourcePack : public pisk::system::ResourcePack {
	class FileDataStream : public pisk::infrastructure::DataStream {

		std::ifstream file;

	public:
		static pisk::infrastructure::DataStreamPtr open(const std::string& rid)
		{
			std::ifstream file(rid.c_str(), std::ios::binary);
			if (file.is_open() == false)
				return nullptr;
			return std::unique_ptr<FileDataStream>(new FileDataStream(std::move(file)));
		}

	private:

		explicit FileDataStream(std::ifstream&& file) :
			file(std::move(file))
		{}

		const std::streamsize size() const {
			std::ifstream& ref = const_cast<std::ifstream&>(file);
			const std::streamoff orig_pos = ref.tellg();
			ref.seekg(0, std::ios::end);
			const std::streamoff content_end = ref.tellg();
			ref.seekg(orig_pos, std::ios::beg);
			return content_end;
		}

		virtual std::size_t tell() const override {
			std::ifstream& ref = const_cast<std::ifstream&>(file);
			if (file.bad())
				return DataStream::error;
			return static_cast<std::size_t>(ref.tellg());
		}

		virtual std::size_t seek(const long pos, const Whence whence) override {
			std::ifstream& ref = const_cast<std::ifstream&>(file);
			if (whence == Whence::begin)
				ref.seekg(pos, std::ios::beg);
			else if (whence == Whence::end)
				ref.seekg(pos, std::ios::end);
			else
				ref.seekg(pos, std::ios::cur);
			if (file.bad())
				return DataStream::error;
			return static_cast<std::size_t>(ref.tellg());
		}

		virtual std::size_t read(const std::size_t count, pisk::infrastructure::DataBuffer& out) final override {
			out.resize(count);
			if (file.bad())
				return DataStream::error;
			file.read(reinterpret_cast<char*>(out.data()), out.size());
			if (file.bad())
				return DataStream::error;
			out.resize(static_cast<std::size_t>(file.gcount()));
			return out.size();
		}

		virtual pisk::infrastructure::DataBuffer readall() const final override {
			std::ifstream& ref = const_cast<std::ifstream&>(file);
			if (file.bad())
				return {};
			const std::size_t orig_pos = static_cast<std::size_t>(ref.tellg());
			const std::size_t stream_size = static_cast<std::size_t>(size());
			ref.seekg(0, std::ios::beg);

			pisk::infrastructure::DataBuffer out;
			out.resize(stream_size);
			ref.read(reinterpret_cast<char*>(out.data()), out.size());
			if (file.bad())
				return {};
			out.resize(static_cast<std::size_t>(ref.tellg()));
			ref.seekg(orig_pos, std::ios::beg);
			return out;
		}
	};

	pisk::utils::keystring resource_prefix;

public:

	explicit FileResourcePack(const pisk::utils::keystring& prefix = ""):
		resource_prefix(prefix)
	{}

	virtual pisk::infrastructure::DataStreamPtr open(const std::string& rid) const noexcept threadsafe final override
	{
		const auto& path = resource_prefix.empty() ? rid : resource_prefix.get_content() + '/' + rid;
		pisk::logger::debug("filepack", "Try to open '{}'", path);
		return FileDataStream::open(path);
	}
};

