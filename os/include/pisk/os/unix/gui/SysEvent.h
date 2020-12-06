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

#include "../../SysEventDispatcher.h"
#include "x11.h"

namespace pisk
{
namespace os
{
	using OSSysEvent = XEvent;
	using OSSysEventDispatcher = SysEventDispatcher<OSSysEvent>;
	using OSSysEventDispatcherPtr = SysEventDispatcherPtr<OSSysEvent>;
	using OSSysEventHandler = SysEventHandler<OSSysEvent>;
}
}


