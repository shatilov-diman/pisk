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
#include <pisk/infrastructure/Logger.h>

#include <pisk/os/windows/gui/SysEvent.h>
#include <pisk/os/windows/gui/WindowManager.h>

namespace pisk
{
namespace os
{
namespace impl
{
	class Window final :
		public windows_gui::Window
	{
		SysEventDispatcherPtr<OSSysEvent> dispatcher;
		HWND window;

	public:
		Window(SysEventDispatcherPtr<OSSysEvent> _dispatcher, HWND wnd) :
			dispatcher(_dispatcher),
			window(wnd)
		{
			if (dispatcher == NULL)
				throw infrastructure::NullPointerException();
			if (window == NULL)
				throw infrastructure::NullPointerException();
			pisk::logger::debug("window_manager", "Window {} was made; handle: {}", this, window);
		}

		virtual ~Window()
		{
			close_handle_impl();
			pisk::logger::debug("window_manager", "Window {} was destroyed", this);
		}

	public:
		using WindowPtr = std::shared_ptr<Window>;

		void close_handle()
		{
			run_from_dispatcher_thread_sync([this]() {
				close_handle_impl();
			});
		}

	private:
		void close_handle_impl()
		{
			if (is_handle_closed())
				return;

			pisk::logger::debug("window_manager", "Window {} is closing; handle: {}", this, window);
			::DestroyWindow(window);
		}

	public:
		void on_close_handle()
		{
			if (not is_handle_closed())
			{
				pisk::logger::debug("window_manager", "Window {} is closing; handle: {}", this, window);
				window = NULL;
			}
		}

		HWND get_handle() const
		{
			return window;
		}

		bool is_handle_closed() const threadsafe
		{
			return window == NULL;
		}

	public://Releasable
		virtual void release() final override
		{
			pisk::logger::debug("window_manager", "Window {} is destroying", this);
			delete this;
		}

	public://os::Window
		virtual Size get_window_size() const threadsafe final override
		{
			return run_from_dispatcher_thread_sync([this]() {
				return get_window_size_impl();
			});
		}
		Size get_window_size_impl() const
		{
			if (is_handle_closed())
				return {};
			RECT rect {};
			if (::GetWindowRect(window, &rect) == FALSE)
			{
				pisk::logger::warning("window_manager", "Window {} cannot get size, {}", this, ::GetLastError());
				return {};
			}
			return {static_cast<unsigned int>(rect.right - rect.left), static_cast<unsigned int>(rect.bottom - rect.top)};
		}

	public://os::MultitaskWindow
		virtual bool is_fullscreen_mode_on() const threadsafe final override
		{
			return run_from_dispatcher_thread_sync([this]() {
				return is_fullscreen_mode_on_impl();
			});
		}
		bool is_fullscreen_mode_on_impl() const
		{
			if (is_handle_closed())
				return false;
			const DWORD ex_style = GetWindowLong(window, GWL_EXSTYLE);
			return (ex_style & WS_EX_TOPMOST) == WS_EX_TOPMOST;
		}

		virtual bool set_fullscreen_mode(const bool turn_on) threadsafe final override
		{
			pisk::logger::debug("window_manager", "Window {} set fullscreen {}", window, turn_on ? "on" : "off");
			return run_from_dispatcher_thread_sync([this, turn_on]() {
				return set_fullscreen_mode_impl(turn_on);
			});
		}
		bool set_fullscreen_mode_impl(const bool turn_on) const
		{
			if (is_handle_closed())
				return false;

			if (is_fullscreen_mode_on_impl() == turn_on)
				return true;

			return turn_on ? set_fullscreen_mode_on() : set_fullscreen_mode_off();
		}
	private:
		bool set_fullscreen_mode_on() const
		{
			const DWORD style = GetWindowLong(window, GWL_STYLE);
			const DWORD ex_style = GetWindowLong(window, GWL_EXSTYLE);
			SetWindowLongPtr(window, GWL_STYLE, (style | WS_POPUP) & ~(WS_CAPTION | WS_THICKFRAME));
			SetWindowLongPtr(window, GWL_EXSTYLE, (ex_style | WS_EX_APPWINDOW) & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
			ShowWindow(window, SW_MAXIMIZE);
			SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
			return true;
		}

		bool set_fullscreen_mode_off() const
		{
			const DWORD style = GetWindowLong(window, GWL_STYLE);
			const DWORD ex_style = GetWindowLong(window, GWL_EXSTYLE);
			SetWindowLongPtr(window, GWL_STYLE, (style | WS_CAPTION | WS_THICKFRAME) & ~(WS_POPUP ));
			SetWindowLongPtr(window, GWL_EXSTYLE, (ex_style | WS_EX_STATICEDGE) & ~(WS_EX_APPWINDOW));
			ShowWindow(window, SW_RESTORE);
			SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
			return true;
		}

	private:
		template <typename Callee>
		auto run_from_dispatcher_thread_sync(Callee&& runnable) const threadsafe -> decltype(runnable())
		{
			return dispatcher->run_from_dispatcher_thread_sync(std::forward<Callee>(runnable));
		}

		template <typename Callee>
		auto run_from_dispatcher_thread_async(Callee&& runnable) const threadsafe -> decltype(dispatcher->run_from_dispatcher_thread_async(std::forward<Callee>(runnable)))
		{
			return dispatcher->run_from_dispatcher_thread_async(std::forward<Callee>(runnable));
		}
	};
	using WindowPtr = Window::WindowPtr;

}//namespace impl
}//namespace os
}//namespace pisk

