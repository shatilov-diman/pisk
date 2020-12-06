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

#include "../defines.h"
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdarg>
#include <algorithm>
#include <type_traits>

#include "../infrastructure/Exception.h"

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

	template<typename Range>
	Range split(const std::string& source, const std::string& splitter)
	{
		Range out;

		std::string tmp;
		for (const auto& ch : source)
		{
			if (splitter.find(ch) == std::string::npos)
				tmp.push_back(ch);
			else if (not tmp.empty())
				out.emplace_back(std::move(tmp));
		}
		if (not tmp.empty())
			out.emplace_back(std::move(tmp));
		return out;
	}
}//namespace algorithm

namespace string
{
	namespace details
	{
		constexpr const char hex_prefix[] = "0x";

		template <typename TArg>
		inline std::size_t get_to_string_size(const TArg&, std::enable_if_t<std::is_arithmetic<TArg>::value>* = 0)
		{
			static_assert(std::numeric_limits<TArg>::digits10 != 0, "qwe");
			return std::numeric_limits<TArg>::digits10;
		}
		template <typename TArg>
		inline std::size_t get_to_string_size(const TArg&, std::enable_if_t<std::is_enum<TArg>::value>* = 0)
		{
			using subtype_t = std::underlying_type_t<TArg>;
			static_assert(std::numeric_limits<subtype_t>::digits10 != 0, "qwe");
			return std::numeric_limits<subtype_t>::digits10;
		}
		template <typename TArg>
		inline std::size_t get_to_string_size(const TArg&, std::enable_if_t<std::is_pointer<TArg>::value>* = 0)
		{
			return sizeof(hex_prefix)-1 + get_to_string_size(static_cast<std::size_t>(0));
		}
		template <typename TArg>
		inline std::size_t get_to_string_size(const TArg&, std::enable_if_t<std::is_same<TArg, bool>::value>* = 0)
		{
			return 5;
		}
		inline std::size_t get_to_string_size(const char&)
		{
			return 1;
		}
		template <std::size_t N>
		inline std::size_t get_to_string_size(const char (&arg)[N])
		{
			return N * get_to_string_size(arg[0]);
		}
		inline std::size_t get_to_string_size(const char* arg)
		{
			return std::strlen(arg);
		}

		inline std::size_t get_to_string_size(const std::string& arg)
		{
			return arg.size();
		}

		inline std::size_t get_total_size()
		{
			return 0;
		}

		template <typename TArg, typename ... TArgs>
		inline std::size_t get_total_size(const TArg& arg, const TArgs& ... args)
		{
			return get_to_string_size(arg) + get_total_size(args...);
		}

		template <typename TArg>
		inline std::string to_string(const TArg& arg, std::enable_if_t<std::is_arithmetic<TArg>::value>* = 0)
		{
			return std::to_string(arg);
		}
		template <typename TArg>
		inline std::string to_string(const TArg& arg, std::enable_if_t<std::is_enum<TArg>::value>* = 0)
		{
			using subtype_t = std::underlying_type_t<TArg>;
			return to_string(static_cast<subtype_t>(arg));
		}
		template <typename TArg>
		inline std::string to_string(const TArg& arg, std::enable_if_t<std::is_pointer<TArg>::value>* = 0)
		{
			return hex_prefix + std::to_string(reinterpret_cast<std::size_t>(arg));
		}
		template <typename TArg>
		inline std::string to_string(const TArg& arg, std::enable_if_t<std::is_same<TArg, bool>::value>* = 0)
		{
			return arg ? "true" : "false";
		}
		inline std::string to_string(const char& arg)
		{
			return std::string(1, arg);
		}

		template <std::size_t N>
		inline std::string to_string(const char (&str)[N])
		{
			return str;
		}
		inline std::string to_string(const char* str)
		{
			return str;
		}
		inline std::string to_string(const std::string& arg)
		{
			return arg;
		}
		inline std::string to_string(std::string&& arg)
		{
			return std::move(arg);
		}

		template <typename TArg>
		inline std::size_t to_string(std::string& out, std::size_t opos, const TArg& arg)
		{
			const auto& tmp = to_string(arg);
			for (const char& ch : tmp)
				out[opos++] = ch;
			return tmp.size();
		}

		inline std::size_t format(const std::string& f, std::string& out, std::size_t fpos, std::size_t opos)
		{
			const std::size_t count = f.size();
			const std::size_t ocount = out.size();
			for (;fpos != count; ++fpos)
			{
				if (opos >= ocount)
					throw infrastructure::OutOfRangeException();
				out[opos++] = f[fpos];
			}
			return opos;
		}

		template <typename TArg, typename ... TArgs>
		inline std::size_t format(const std::string& f, std::string& out, std::size_t fpos, std::size_t opos, TArg&& arg, TArgs&& ... args)
		{
			const std::size_t count = f.size();
			const std::size_t ocount = out.size();
			for (;fpos != count; ++fpos)
			{
				const char ch = f[fpos];
				if (ch == '\x0')
					break;
				if (ch == '{')
				{
					if (f[fpos+1] == '}')
					{
						opos += to_string(out, opos, arg);
						return format(f, out, fpos+2, opos, std::forward<TArgs>(args)...);
					}
				}

				if (opos >= ocount)
					throw infrastructure::OutOfRangeException();

				out[opos++] = ch;
			}
			return opos;
		}
	}//namespace details

	template <typename ... TArgs>
	inline std::string format(const std::string& f, TArgs&& ... args)
	{
		std::string out;
		out.resize(f.size() + details::get_total_size(args...));
		const std::size_t count = details::format(f, out, 0, 0, std::forward<TArgs>(args)...);
		out.resize(count);
		return out;
	}
}//namespace string

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
