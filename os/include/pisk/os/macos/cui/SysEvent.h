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

namespace pisk
{
namespace os
{
	struct Event
	{
		enum class Type
		{
			Unknown,
			Keyboard,
		};

		Type type = Type::Unknown;
		std::size_t param1 = 0;
		std::size_t param2 = 0;
		void* paramp = nullptr;
	};

	using OSSysEvent = Event;
	using OSSysEventDispatcher = SysEventDispatcher<OSSysEvent>;
	using OSSysEventDispatcherPtr = SysEventDispatcherPtr<OSSysEvent>;
	using OSSysEventHandler = SysEventHandler<OSSysEvent>;
}
}

