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

#include <functional>
#include <memory>

#include "../infrastructure/Exception.h"
#include "algorithm_utils.h"

namespace pisk
{
namespace utils
{
	class keystring
	{
		std::shared_ptr<const std::string> content;
		std::size_t hash;

		static std::size_t calc_hash(const std::string& str)
		{
			std::hash<std::string> hasher;
			return hasher(str);
		}
		static std::size_t get_empty_hash()
		{
			static std::size_t hash = calc_hash("");
			return hash;
		}

	public:
		using value_type = std::string::value_type;
		using const_interator = std::string::const_iterator;

		keystring() :
			hash(get_empty_hash())
		{}
		keystring(const char* str) :
			content(std::make_shared<const std::string>(str == nullptr ? "" : str)),
			hash(calc_hash(*content))
		{}
		explicit keystring(std::string&& str) :
			content(std::make_shared<const std::string>(std::move(str))),
			hash(calc_hash(*content))
		{}
		explicit keystring(const std::string& str) :
			content(std::make_shared<const std::string>(str)),
			hash(calc_hash(*content))
		{}
		keystring(const keystring& str) = default;

		bool empty() const {
			return hash == get_empty_hash() && (content == nullptr || content->empty());
		}
		std::size_t size() const {
			return content == nullptr ? 0 : content->size();
		}
		std::size_t get_hash() const {
			return hash;
		}
		const std::string& get_content() const {
			static const std::string empty;
			if (content == nullptr)
				return empty;
			return *content;
		}
		const std::string& operator*() const {
			return get_content();
		}
		const char* c_str() const {
			return content == nullptr ? "" : content->c_str();
		}
		const char* data() const {
			return content == nullptr ? "" : content->data();
		}

		const_interator begin() const
		{
			if (content == nullptr)
				return {};
			return content->begin();
		}
		const_interator end() const
		{
			if (content == nullptr)
				return {};
			return content->end();
		}

		bool operator == (const keystring& str) const {
			if (this == &str)
				return true;
			if (hash != str.hash)
				return false;
			if (content == nullptr)
				return str.content == nullptr || str.content->empty();
			if (str.content == nullptr)
				return content->empty();
			return *content == *str.content;
		}
		bool operator == (const std::string& other) const {
			if (content == nullptr)
				return other == "";
			return *content == other;
		}
		bool operator == (const char* other) const {
			if ((content == nullptr or content->empty())
			    and (other == nullptr or *other == '\x0'))
				return true;
			if (other == nullptr or content == nullptr)
				return false;
			return *content == other;
		}
		template <typename Other>
		bool operator != (const Other& other) const {
			return not (*this == other);
		}

		bool operator < (const keystring& str) const {
			return hash < str.hash;
		}

		keystring operator + (const keystring& str) const {
			return keystring {*content + *str.content};
		}
		keystring operator + (const char* str) const {
			if (str == nullptr or *str == '\x0')
				return *this;
			return keystring {*content + str};
		}
		template <typename Other>
		keystring operator + (const Other& other) const {
			return keystring {*content + other};
		}

		template <typename Other, typename = typename utils::disable_if<std::is_same<Other, keystring>::value>::type>
		friend bool operator == (const Other& left, const keystring& right)
		{
			return right.operator == (left);
		}
		template <typename Other, typename = typename utils::disable_if<std::is_same<Other, keystring>::value>::type>
		friend bool operator != (const Other& left, const keystring& right)
		{
			return right.operator != (left);
		}

		friend keystring operator + (const char* left, const keystring& right) {
			if (left == nullptr or *left == '\x0')
				return right;
			return keystring {left + *right.content};
		}
		template <typename Other>
		friend keystring operator + (const Other& left, const keystring& right)
		{
			return keystring {left + *right.content};
		}
	};

	inline std::size_t get_to_string_size(const keystring& arg)
	{
		return arg.size();
	}

	inline keystring to_string(const keystring& arg)
	{
		return arg;
	}
}
}

namespace std
{
	template<>
	struct hash < pisk::utils::keystring >
	{
		std::size_t operator()(const pisk::utils::keystring& key) {
			return key.get_hash();
		}
	};
}
