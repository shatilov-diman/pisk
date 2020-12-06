
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

#include <pisk/defines.h>

namespace pisk
{
namespace os
{
	struct Point
	{
		int x = 0;
		int y = 0;
	};
	struct Size
	{
		unsigned int cx = 0;
		unsigned int cy = 0;
	};
	struct Rect
	{
		int x = 0;
		int y = 0;
		unsigned int cx = 0;
		unsigned int cy = 0;

		explicit Rect(const Point& pt)
		{
			x = pt.x;
			y = pt.y;
		}
		explicit Rect(const Size& sz)
		{
			cx = sz.cx;
			cy = sz.cy;
		}
		Rect(const Point& pt, const Size& sz)
		{
			x = pt.x;
			y = pt.y;
			cx = sz.cx;
			cy = sz.cy;
		}

		Point position()
		{
			return {x, y};
		}

		Size size()
		{
			return {cx, cy};
		}
	};
}//namespace os
}//namespace pisk

