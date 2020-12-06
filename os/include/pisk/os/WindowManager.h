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
#include <pisk/utils/signaler.h>
#include <pisk/tools/ComponentPtr.h>
#include <pisk/infrastructure/Logger.h>

#include "Window.h"

#include <set>

namespace pisk
{
namespace os
{
	class WindowManager :
		public core::Component
	{
	public:
		constexpr static const char* uid = "window_manager";

		utils::signaler<WindowPtr> window_create;
		utils::signaler<WindowPtr> window_destroy;

		virtual std::set<WindowPtr> get_windows() threadsafe const = 0;
	};
	using WindowManagerPtr = tools::InterfacePtr<WindowManager>;
}
}

