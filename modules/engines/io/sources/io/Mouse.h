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

#include <pisk/utils/signaler.h>
#include <pisk/tools/ComponentPtr.h>

#include <set>

namespace pisk
{
namespace io
{

class Mouse:
	public core::Component
{
	virtual void release() final override
	{
		delete this;
	}

public:
	constexpr static const char* uid = "mouse";

	enum class ButtonType {
		Unknown,
		Left,
		Middle,
		Right,
	};
	enum class WheelSide {
		Unknown,
		Up,
		Down,
	};

	struct Shift {
		int dx;
		int dy;
	};

	utils::signaler<const ButtonType> up;
	utils::signaler<const ButtonType> down;
	utils::signaler<const WheelSide> wheel;
	utils::signaler<const Shift> move;
};
using MousePtr = tools::InterfacePtr<Mouse>;

}
}

