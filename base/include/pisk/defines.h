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
