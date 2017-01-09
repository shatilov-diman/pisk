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
#include "../utils/noncopyable.h"
#include "../utils/signaler.h"
#include "../utils/keystring.h"

namespace pisk
{
namespace tools
{
namespace ioevents
{
	static utils::keystring keyboard("keyboard");
	static utils::keystring mouse("mouse");
	static utils::keystring touchscreen("touchscreen");
	static utils::keystring joystick("joystick");

	struct IOKey
	{
		enum class VKEY;
		struct Event
		{
			utils::keystring device;
			VKEY key;
		};
		utils::signaler<Event> Toogle;
		utils::signaler<Event> Down;
		utils::signaler<Event> Up;
	};
	enum class IOKey::VKEY
	{
		//Keyboard
		Backspace = 0x08, Tab = 0x09, Enter = 0x0d, Shift = 0x10, Ctrl = 0x11, ALt = 0x12, Pause = 0x13, Capslock = 0x14, Escape = 0x1b, Space = 0x20,
		End = 0x23, Home = 0x24,
		Left = 0x25, Up, Right, Down,
		Print = 0x2a, Inert = 0x2d, Delete = 0x2e,
		_0 = 0x30, _1, _2, _3, _4, _5, _6, _7, _8, _9,
		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		num_0 = 0x60, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9,
		Multiply = 0x6a, Add = 0x6b, Subtrac = 0x6d, Devide = 0x6f,
		f1 = 0x70, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24,
		NumLock = 0x90, ScrollLock = 0x91,
		LShift = 0xa0, RShift, LCtrl, RCtrl, LAlt, RAlt,
		VolumeMute = 0xad, VolumeDown, VolumeUp,
		Semicolon = 0xba, Plus = 0xbb, Comma = 0xbc, Minus = 0xbd, Dot = 0xbe, Slash = 0xbf, Backslash = 0xdc, Tilde = 0xc0, Quote = 0xde,

		//Mouse
		Mouse_Left = 0x01,
		Mouse_Right = 0x02,
		Mouse_Middle = 0x10,

		__max = 0xff,
	};

	struct IOType
	{
		struct Event
		{
			utils::keystring device;
			char symbol[6];//UTF-8
		};
		utils::signaler<Event> Symbol;
	};

	struct IOMove
	{
		struct Event
		{
			utils::keystring device;
			uint32_t x;
			uint32_t y;
		};
		utils::signaler<Event> Move;
	};

	struct IOTilt
	{
		enum class Axis
		{
			_1 = 1,
			OX = 1,
			_2 = 2,
			OY = 2,
			_3 = 3,
			OZ = 3,
			_4 = 4, _5 = 5, _6 = 6, _7 = 7, _8 = 8, _9 = 9,
		};
		struct Event
		{
			utils::keystring device;
			float deviation;//radian
			Axis axis;
		};
		utils::signaler<Event> KeyDown;
	};

	struct IOWheel
	{
		struct Event
		{
			utils::keystring device;
			uint32_t delta;
		};
		utils::signaler<Event> Toogle;
	};

	struct IOScroll
	{
		struct Event
		{
			utils::keystring device;
			float percents;
		};
		utils::signaler<Event> Toogle;
	};
}
}
}
