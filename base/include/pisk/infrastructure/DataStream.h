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

#include "DataBuffer.h"
#include "../utils/noncopyable.h"

#include "Exception.h"

#include <memory>
#include <limits>

namespace pisk
{
namespace infrastructure
{
	class DataStream :
		public utils::noncopyable
	{
	public:
		using DataStreamPtr = std::unique_ptr<DataStream>;

		enum class Whence
		{
			begin,
			current,
			end,
		};

		static constexpr const std::size_t error = std::numeric_limits<std::size_t>::max();

		virtual ~DataStream() {}

		virtual std::size_t tell() const = 0;

		virtual std::size_t seek(const long pos, const Whence whence) = 0;

		virtual std::size_t read(const std::size_t count, DataBuffer& out) = 0;

		virtual DataBuffer readall() const = 0;
	};
	using DataStreamPtr = DataStream::DataStreamPtr;
}
}

