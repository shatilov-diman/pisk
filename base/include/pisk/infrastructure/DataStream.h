// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
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

