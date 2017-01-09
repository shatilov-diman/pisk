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

#ifdef _MSC_VER
#	if _MSC_VER < 1900
#		define _ALLOW_KEYWORD_MACROS
#		define noexcept throw()
#	endif
#	define not !
#	define and &&
#	define or ||
#	define _CRT_SECURE_NO_WARNINGS
#	define EXPORT __declspec(dllexport)
#	pragma warning(disable: 4503) //(W1 !!!) decorated name length exceeded, name was truncated
#	pragma warning(disable: 4702) //unreachable code
#elif __GNUC__
#	define __cdecl
#	define __stdcall
#	define EXPORT
#endif

#define threadsafe

#define UNUSED(x) static_cast<void>(x)
