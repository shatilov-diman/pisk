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

#include <pisk/os/unix/gui/x11.h>
#include <pisk/os/unix/gui/SysEvent.h>
#include <pisk/os/unix/gui/OsAppInstance.h>
#include <pisk/os/unix/gui/WindowManager.h>

namespace pisk
{
namespace os
{
namespace impl
{
	class Window final :
		public unix_gui::Window
	{
		const int _NET_WM_FORMAT = 32;
		const int _NET_WM_EVENT_REMOVE = 0;
		const int _NET_WM_EVENT_ADD = 1;
		const int _NET_WM_EVENT_TOOGLE = 1;

		XDisplay display;
		XWindow window;

		Window(XDisplay display, XWindow wnd) :
			display(display),
			window(wnd)
		{
			if (display == nullptr or window == 0)
				throw infrastructure::NullPointerException();
			pisk::logger::debug("window_manager", "Window {} was made; handle: {}", this, window);
		}

		virtual ~Window()
		{
			if (not is_handle_closed())
				close_handle();
			pisk::logger::debug("window_manager", "Window {} was destroyed", this);
		}

	public:
		using WindowPtr = std::shared_ptr<Window>;

		static Size get_screen_size(XDisplay display, unsigned int screen_num) threadsafe
		{
			XScreen screen = XScreenOfDisplay(display, screen_num);
			const int width = WidthOfScreen(screen);
			const int height = HeightOfScreen(screen);
			return {static_cast<unsigned int>(width), static_cast<unsigned int>(height)};
		}

		static WindowPtr make(XDisplay display, const XVisualInfo& vi, const long event_mask, const utils::keystring& name)
		{
			const unsigned int border_width = 2U;

			const auto screen_num = DefaultScreen(display);
			const auto screen_size = get_screen_size(display, screen_num);
			auto root_window = DefaultRootWindow(display);

			XSetWindowAttributes swa;
			swa.colormap = XCreateColormap(display, root_window, vi.visual, AllocNone);
			swa.event_mask = event_mask;

			XWindow wnd = XCreateWindow(
				display,
				root_window,
				0, 0, screen_size.cx, screen_size.cy,
				border_width,
				vi.depth,
				InputOutput,
				vi.visual,
				CWColormap | CWEventMask, &swa
			);
			if (wnd == 0)
			{
				pisk::logger::error("window_manager", "Unable to create XWindow");
				return {};
			}

			pisk::logger::debug("window_manager", "XWindow made: handle: {}; screen: {}; width: {}; height: {};", wnd, screen_num, screen_size.cx, screen_size.cy);

			set_invisible_cursor(display, wnd);

			XMapWindow(display, wnd);

			XStoreName(display, wnd, name.c_str());

			Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
			if (wm_delete_window != None)
				XSetWMProtocols(display, wnd, &wm_delete_window, 1);

			XFlush(display);

			return tools::make_shared_releasable_raw<Window>(new Window(display, wnd));
		}

		static Cursor set_invisible_cursor(XDisplay display, XWindow wnd)
		{
			char data[1] = {0};
			XColor dummy{};
			Pixmap blank = XCreateBitmapFromData(display, wnd, data, 1, 1);
			Cursor cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
			XDefineCursor(display, wnd, cursor);
			XFreeCursor(display, cursor);
			XFreePixmap(display, blank);
			return cursor;
		}

		void close_handle()
		{
			if (not is_handle_closed())
			{
				pisk::logger::debug("window_manager", "Window {} is closing; handle: {}", this, window);
				XDestroyWindow(display, window);
				window = 0;
			}
		}

		XWindow get_handle() const
		{
			return window;
		}

		bool is_handle_closed() const threadsafe
		{
			return window == 0;
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
			if (is_handle_closed())
				return {};
			XWindowAttributes atts;
			XGetWindowAttributes(display, window, &atts);
			return {static_cast<unsigned int>(atts.width), static_cast<unsigned int>(atts.height)};
		}

	public://os::MultitaskWindow
		virtual bool is_fullscreen_mode_on() const threadsafe final override
		{
			return is_net_wm_substate_enabled("_NET_WM_STATE_FULLSCREEN");
		}

		virtual bool set_fullscreen_mode(const bool on) threadsafe final override
		{
			return set_net_wm_substate("_NET_WM_STATE_FULLSCREEN", on);
		}

		bool is_net_wm_substate_enabled(const std::string& substate_atom_name) const threadsafe
		{
			Atom wm_state_atom = XInternAtom(display, "_NET_WM_STATE", False);
			Atom wm_substate_atom = XInternAtom(display, substate_atom_name.c_str(), False);
			if (wm_state_atom == None or wm_substate_atom == None)
			{
				pisk::logger::warning("window_manager", "Atom '{}' not found, window {}, handle: {}", substate_atom_name, this, this->window);
				return false;
			}
			const auto& atoms = get_net_wm_xa_atom_properties(wm_state_atom);
			for (const auto& atom : atoms)
				if (atom == wm_substate_atom)
					return true;
			return false;
		}

		bool set_net_wm_substate(const std::string& substate_atom_name, const bool on) threadsafe
		{
			Atom wm_state_atom = XInternAtom(display, "_NET_WM_STATE", False);
			Atom wm_substate_atom = XInternAtom(display, substate_atom_name.c_str(), True);
			if (wm_state_atom == None or wm_substate_atom == None)
			{
				pisk::logger::error("window_manager", "Unable to set fullscreen mode for window {} (handle: {})", this, this->window);
				return false;
			}
			auto atoms = get_net_wm_xa_atom_properties(wm_state_atom);
			auto found = std::find(atoms.begin(), atoms.end(), wm_substate_atom);
			if (on == false)
			{
				if (found == atoms.end())
					return false;
				atoms.erase(found);
			}
			else
			{
				if (found != atoms.end())
					return false;
				atoms.push_back(wm_substate_atom);
			}
			atoms.push_back(None);
			XChangeProperty(display, window, wm_state_atom, XA_ATOM, _NET_WM_FORMAT, PropModeReplace, reinterpret_cast<unsigned char*>(atoms.data()), atoms.size()-1);

			send_net_wm_notification(wm_state_atom, wm_substate_atom, on);
			pisk::logger::info("window_manager", "Set fullscreen mode {} (window {}; handle: {})", on ? "ON" : "OFF", this, this->window);
			return true;
		}

		void send_net_wm_notification(Atom wm_state_atom, Atom wm_substate_atom, const bool on) threadsafe
		{
			XEvent xev = {0};
			xev.type = ClientMessage;
			xev.xclient.window = window;
			xev.xclient.message_type = wm_state_atom;
			xev.xclient.format = _NET_WM_FORMAT;
			xev.xclient.data.l[0] = on ? _NET_WM_EVENT_ADD : _NET_WM_EVENT_REMOVE;
			xev.xclient.data.l[1] = wm_substate_atom;
			XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask, &xev);
		}

		std::vector<Atom> get_net_wm_xa_atom_properties(Atom atom) const threadsafe
		{
			if (atom == None)
				return {};

			const unsigned int retrived_data_offset = 0;
			const unsigned int retrived_data_len = 1024;
			Atom actual_type = None;
			int actual_format = 0;
			unsigned long count = 0;
			unsigned long bytes = 0;
			unsigned char* prop = nullptr;
			const auto result = XGetWindowProperty(display, window, atom, retrived_data_offset, retrived_data_len, false, XA_ATOM, &actual_type, &actual_format, &count, &bytes, &prop);
			if (result != Success)
			{
				pisk::logger::error("window_manager", "Unable to check fullscreen mode for window {} (handle: {}): unable to read '_NET_WM_STATE' property", this, this->window);
				if (prop != nullptr)
					XFree(prop);
				return {};
			}
			if (prop == nullptr)
			{
				pisk::logger::error("window_manager", "Unable to check fullscreen mode for window {} (handle: {}): unexpected return value", this, this->window);
				return {};
			}
			if (actual_type != XA_ATOM or actual_format != _NET_WM_FORMAT)
			{
				pisk::logger::error("window_manager", "Unable to check fullscreen mode for window {} (handle: {}): unexpected atom format", this, this->window);
				XFree(prop);
				return {};
			}
			std::vector<Atom> out;
			Atom* atoms = reinterpret_cast<Atom*>(prop);
			for (unsigned long i = 0; i < count; ++i)
				out.push_back(atoms[i]);
			XFree(prop);
			return out;
		}
	};
	using WindowPtr = Window::WindowPtr;

}//namespace impl
}//namespace os
}//namespace pisk

