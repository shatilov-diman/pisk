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

#include "defines.h"

#ifdef _MSC_VER
#	define DISABLE_IGLOO_WARNINGS __pragma(warning(push,3))
#	define ENABLE_IGLOO_WARNINGS __pragma(warning(pop))
#else
#	define DISABLE_IGLOO_WARNINGS _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#	define ENABLE_IGLOO_WARNINGS _Pragma("GCC diagnostic pop")
#endif

DISABLE_IGLOO_WARNINGS

#include <igloo/igloo.h>
#include <igloo/igloo_alt.h>
#include <igloo/igloo_framework.h>

ENABLE_IGLOO_WARNINGS

#define AssertThrowsEx(x, y) AssertThrows(x, y)

