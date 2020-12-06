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

#include <pisk/tools/OsAppInstance.h>
#include <pisk/os/WindowManager.h>

#include <pisk/os/android/jni_os_headers.h>

namespace pisk
{
namespace os
{
namespace android
{
	using Window = os::Window;
	using WindowPtr = tools::shared_relesable_ptr<Window>;

	class WindowManager :
		public os::WindowManager
	{
	public:
		virtual struct ANativeWindow* get_handle(const os::WindowPtr& window) threadsafe const = 0;
	};
	using WindowManagerPtr = tools::InterfacePtr<WindowManager>;
}//namespace unix_gui
}//namespace os
}//namespace pisk

