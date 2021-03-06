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

#include "MainLoopSignal.h"

namespace pisk
{
namespace os
{

template <typename MainLoopBase>
std::sig_atomic_t pisk::os::MainLoopSignal<MainLoopBase>::lastsignal = 0;

}//namespace os
}//namespace pisk

