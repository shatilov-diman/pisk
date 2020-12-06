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

#include <Windows.h>

namespace pisk
{
namespace os
{
	struct MSG_EX :
		public MSG
	{
		LRESULT lResult;
	};

	using OSSysEvent = MSG_EX;
	using OSSysEventDispatcher = SysEventDispatcher<OSSysEvent>;
	using OSSysEventDispatcherPtr = SysEventDispatcherPtr<OSSysEvent>;
	using OSSysEventHandler = SysEventHandler<OSSysEvent>;
}
}

