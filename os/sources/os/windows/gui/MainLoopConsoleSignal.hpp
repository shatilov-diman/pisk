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

#include "MainLoopConsoleSignal.h"

namespace pisk
{
namespace os
{
namespace impl
{
	template <typename TMainLoopBase>
	std::atomic_bool MainLoopConsoleSignal<TMainLoopBase>::stop {false};
}//namespace impl
}//namespace os
}//namespace pisk

