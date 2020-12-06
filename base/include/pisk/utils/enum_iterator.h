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

#include "../infrastructure/Exception.h"

namespace pisk
{
namespace utils
{
namespace iterators
{
	template <typename Enum>
	class enum_iterator
	{
		Enum value;
		Enum last;
		bool end;
	public:
		enum_iterator():
			value(Enum()),
			last(Enum()),
			end(true)
		{}
		enum_iterator(const Enum value, const Enum last):
			value(value),
			last(last),
			end(false)
		{}

		Enum operator*() const
		{
			if (end == true)
				throw infrastructure::OutOfRangeException();
			return value;
		}

		enum_iterator<Enum>& operator++()
		{
			if (end == true)
				throw infrastructure::OutOfRangeException();
			if (value == last)
				end = true;
			else
			{
				if (value < last)
					value = static_cast<Enum>(static_cast<typename std::underlying_type<Enum>::type>(value) + 1);
				else if (value > last)
					value = static_cast<Enum>(static_cast<typename std::underlying_type<Enum>::type>(value) - 1);
			}
			return *this;
		}

		bool operator == (const enum_iterator& iter) const
		{
			if (end or iter.end)
				return end == iter.end;
			return value == iter.value;
		}
		bool operator != (const enum_iterator& iter) const
		{
			return not(*this == iter);
		}
	};

	template <typename Enum>
	class iterable_enum_wrapper
	{
		const Enum first;
		const Enum last;
	public:
		iterable_enum_wrapper(const Enum first, const Enum last):
			first(first),
			last(last)
		{
			static_assert(std::is_enum<Enum>::value, "iterable_enum_wrapper accept only enums");
		}
		enum_iterator<Enum> begin() const
		{
			return {first, last};
		}
		enum_iterator<Enum> end() const
		{
			return {};
		}
	};
	template <typename Enum>
	inline iterable_enum_wrapper<Enum> range(const Enum first, const Enum last)
	{
		return {first, last};
	}
	template <typename Enum>
	inline iterable_enum_wrapper<Enum> range_all();
}
}
}

