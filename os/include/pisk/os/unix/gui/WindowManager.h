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
#include <pisk/utils/keystring.h>

#include <pisk/tools/OsAppInstance.h>
#include <pisk/os/WindowManager.h>

#include "x11.h"

namespace pisk
{
namespace os
{
namespace unix_gui
{
	class Window :
		public os::Window,
		public os::CloseableWindow,
		public os::TaskbarWindow
	{};
	using WindowPtr = tools::shared_relesable_ptr<Window>;

	class WindowManager :
		public os::WindowManager
	{
	public:
		virtual XDisplay get_display() const threadsafe = 0;

		virtual WindowPtr make_window(const XVisualInfo& vi, const utils::keystring& name) threadsafe = 0;

		virtual bool destroy_window(const os::WindowPtr& window) threadsafe = 0;

		virtual XWindow get_handle(const os::WindowPtr& window) const threadsafe = 0;
	};
	using WindowManagerPtr = tools::InterfacePtr<WindowManager>;
}//namespace unix_gui
}//namespace os
}//namespace pisk

