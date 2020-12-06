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

namespace pisk
{
namespace utils
{
	struct noncopyable
	{
		noncopyable() = default;
		noncopyable(const noncopyable&) = delete;
		noncopyable(noncopyable&&) = default;
		noncopyable& operator=(const noncopyable&) = delete;
		noncopyable& operator=(noncopyable&&) = default;
	};
	struct nonmoveable
	{
		nonmoveable() = default;
		nonmoveable(const nonmoveable&) = default;
		nonmoveable(nonmoveable&&) = delete;
		nonmoveable& operator=(const nonmoveable&) = default;
		nonmoveable& operator=(nonmoveable&&) = delete;
	};
}
}
