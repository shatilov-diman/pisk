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

#include "../defines.h"
#include <cstdio>
#include <string>
#include <cstdarg>

namespace pisk
{
template<typename Type, std::size_t Size>
std::size_t countof(Type (&arr)[Size])
{
	UNUSED(arr);
	return Size;
}

namespace utils
{
	template <bool B, typename T = void>
	struct disable_if {
		typedef T type;
	};
	template <typename T>
	struct disable_if<true,T> {
	};

namespace algorithm
{
	template<typename Range, typename Item>
	Item join(const Range& range, const Item& linker)
	{
		const bool linker_not_empty = linker.empty() == false;
		Item out;

		bool first = true;
		for (const auto& item : range)
		{
			if (first)
				first = false;
			else if (linker_not_empty)
				std::copy(linker.begin(), linker.end(), std::back_inserter(out));
			std::copy(item.begin(), item.end(), std::back_inserter(out));
		}
		return out;
	}
}

namespace string
{
	template <size_t buffer_limit = 1024, typename String, typename ...TArgs>
	String format(const String& format, TArgs... args)
	{
		typedef typename String::value_type TChar;
		TChar buffer[buffer_limit];
		snprintf(buffer, countof(buffer), format.data(), args...);
		return buffer;
	}
}

namespace iterators
{
	template <typename T>
	class iterate_backwards
	{
		const T &t;
		iterate_backwards& operator = (const iterate_backwards&);
	public:
		explicit iterate_backwards(const T &t) : t(t) {}
		typename T::const_reverse_iterator begin() const { return t.rbegin(); }
		typename T::const_reverse_iterator end()   const { return t.rend(); }
	};
	template <typename T>
	iterate_backwards<T> backwards(const T &t)
	{
		return iterate_backwards<T>(t);
	}

}
}
}
