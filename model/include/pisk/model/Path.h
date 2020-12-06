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
#include <pisk/utils/algorithm_utils.h>

#include <pisk/infrastructure/Exception.h>

#include <list>

namespace pisk
{
namespace model
{

class PathId
{
	using list_t = std::list<utils::keystring>;
	list_t path;
	utils::keystring path_as_string;
public:

	PathId() = default;
	PathId(const PathId&) = default;
	PathId(PathId&&) = default;

	PathId(const list_t& path) :
		path(path),
		path_as_string(to_keystring_impl(this->path))
	{}

	PathId(list_t&& path) :
		path(std::move(path)),
		path_as_string(to_keystring_impl(this->path))
	{}

	PathId& operator=(const PathId&) = default;
	PathId& operator=(PathId&&) = default;

	static PathId from_string(const std::string& path)
	{
		return PathId {parse(path)};
	}

	static PathId from_keystring(const utils::keystring& path)
	{
		return from_string(*path);
	}

	bool empty() const
	{
		return path.empty();
	}

	PathId add(const utils::keystring& string_path) const
	{
		list_t tmp(path);
		tmp.splice(tmp.end(), parse(*string_path));
		return PathId(std::move(tmp));
	}

	utils::keystring front() const
	{
		if (path.empty())
			return {};
		return path.front();
	}

	PathId tail() const
	{
		list_t tmp = path;
		if (not tmp.empty())
			tmp.pop_front();
		return PathId(std::move(tmp));
	}

	utils::keystring back() const
	{
		if (path.empty())
			return {};
		return path.back();
	}

	PathId parent() const
	{
		list_t tmp = path;
		if (not tmp.empty())
			tmp.pop_back();
		return PathId(std::move(tmp));
	}

	utils::keystring to_keystring() const
	{
		return path_as_string;
	}

	const char* c_str() const
	{
		return path_as_string.c_str();
	}

private:

	static utils::keystring to_keystring_impl(const list_t& path, const std::string& linker = "/")
	{
		return utils::keystring{
			utils::algorithm::join(path, linker)
		};
	}

	static list_t parse(const std::string& path, const std::string& splitter = "/\\")
	{
		return utils::algorithm::split<list_t>(path, splitter);
	}
};

inline std::size_t get_to_string_size(const PathId& arg)
{
	return arg.to_keystring().size();
}

inline utils::keystring to_string(const PathId& arg)
{
	return arg.to_keystring();
}

}//namespace model
}//namespace pisk

