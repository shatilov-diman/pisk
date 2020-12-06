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

#include <pisk/os/WindowManager.h>

#include "os_windows.h"

namespace pisk
{
namespace os
{
namespace windows_gui
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
		using on_window_create_success_t = std::function<void (const WindowPtr& wnd)>;
		using on_window_create_failed_t = std::function<void ()>;

		virtual HINSTANCE get_instance() const threadsafe = 0;

		virtual void make_window(const utils::keystring& name, const on_window_create_success_t& on_success = nullptr, const on_window_create_failed_t& on_failed = nullptr) threadsafe = 0;

		virtual bool destroy_window(const os::WindowPtr& window) threadsafe = 0;

		virtual HWND get_handle(const os::WindowPtr& window) const threadsafe = 0;
	};
	using WindowManagerPtr = tools::InterfacePtr<WindowManager>;
}//namespace unix_gui
}//namespace os
}//namespace pisk

