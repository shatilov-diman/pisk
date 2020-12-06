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


#include <pisk/defines.h>
#include <pisk/tools/ComponentsLoader.h>

#include "Window.h"

#include <mutex>
#include <map>

namespace pisk
{
namespace os
{
namespace impl
{
	class WindowManagerBase :
		public os::unix_gui::WindowManager,
		private os::OSSysEventHandler
	{
		mutable std::mutex mutex;
		std::map<XWindow, WindowPtr> windows;
		XDisplay display;

	public:
		WindowManagerBase(const OSSysEventDispatcherPtr& dispatcher, XDisplay display) :
			os::OSSysEventHandler(dispatcher),
			display(display)
		{
		}

	private://tools::Releasable

		virtual void release() final override
		{
			pisk::logger::debug("window_manager", "WindowManager is destroying");
			close_all_windows();
			delete this;
			pisk::logger::debug("window_manager", "WindowManager was destroyed");
		}

		void close_all_windows()
		{
			std::unique_lock<std::mutex> guard(mutex);
			for (const auto& pair : windows)
				pair.second->close_handle();
			windows.clear();
		}

	protected://os::OSSysEventHandler
		virtual bool handle(const OSSysEvent& event) override
		{
			pisk::logger::debug("window_manager", "XWindow event type: {}", event.type);
			auto window = get_window(event.xany.window);
			if (window == nullptr)
			{
				pisk::logger::warning("window_manager", "Unknown window, handle {}, display {}", event.xany.window, event.xany.display);
				return false;
			}

			switch (event.type)
			{
			case CreateNotify://Event for children windows
				return false;
			case DestroyNotify:
				on_destroy_window(window, event.xdestroywindow);
				return true;
			case ConfigureNotify:
				on_reconfig_window(window, event.xconfigure);
				return true;
			case FocusIn:
				on_activate_window(window, event.xfocus);
				return true;
			case FocusOut:
				on_deactivate_window(window, event.xfocus);
				return true;
			};
			return false;
		}

		void on_destroy_window(const WindowPtr& window, const XDestroyWindowEvent& event) threadsafe
		{
			window->window_deactivate.emit(window);
			this->window_destroy.emit(window);
			remove_window(event.window);
			window->close_handle();
		}

		void on_activate_window(const WindowPtr& window, const XFocusChangeEvent& event) threadsafe
		{
			window->window_activate.emit(window);
		}
		void on_deactivate_window(const WindowPtr& window, const XFocusChangeEvent& event) threadsafe
		{
			window->window_deactivate.emit(window);
		}

		void on_reconfig_window(const WindowPtr& window, const XConfigureEvent& event) threadsafe
		{
			window->window_resize.emit(window);
		}

	private:
		void add_window(const WindowPtr& window) threadsafe
		{
			std::unique_lock<std::mutex> guard(mutex);
			windows[window->get_handle()] = window;
		}

		void remove_window(const XWindow window) threadsafe
		{
			std::unique_lock<std::mutex> guard(mutex);
			windows.erase(window);
		}

	protected:
		WindowPtr get_window(const XWindow window) threadsafe const
		{
			std::unique_lock<std::mutex> guard(mutex);
			auto found = windows.find(window);
			if (found == windows.end())
				return nullptr;
			return found->second;
		}

	private://os::unix_gui::WindowManager
		virtual XDisplay get_display() const threadsafe final override
		{
			return display;
		}

		virtual unix_gui::WindowPtr make_window(const XVisualInfo& vi, const utils::keystring& name) threadsafe final override
		{
			const long event_mask =
				ExposureMask | FocusChangeMask | StructureNotifyMask |
				KeyPressMask | KeyReleaseMask |
				ButtonPressMask | ButtonReleaseMask | PointerMotionMask
				;
			auto window = Window::make(get_display(), vi, event_mask, name);
			if (window == nullptr)
				return nullptr;
			add_window(window);
			this->window_create.emit(window);
			return window;
		}

		virtual XWindow get_handle(const os::WindowPtr& wnd) const threadsafe final override
		{
			WindowPtr window = std::dynamic_pointer_cast<Window>(wnd);
			if (window == nullptr)
			{
				pisk::logger::error("window_manager", "get_handle: unable to get internal interface from Window object {}", wnd.get());
				return 0;
			}
			return window->get_handle();
		}

	private://os::WindowManager
		virtual bool destroy_window(const os::WindowPtr& wnd) threadsafe final override
		{
			WindowPtr window = std::dynamic_pointer_cast<Window>(wnd);
			if (window == nullptr)
			{
				pisk::logger::error("window_manager", "destroy_window: unable to get internal interface from Window object {}", wnd.get());
				return false;
			}
			if (window->is_handle_closed())
			{
				pisk::logger::error("window_manager", "Window {} already destroied", wnd.get());
				return false;
			}
			window->close_handle();
			return true;
		}

		virtual std::set<os::WindowPtr> get_windows() threadsafe const final override
		{
			std::set<os::WindowPtr> out;
			{
				std::unique_lock<std::mutex> guard(mutex);
				for (const auto& pairs : windows)
					out.insert(pairs.second);
			}
			return out;
		}
	};

	class WindowManager :
		public WindowManagerBase
	{
		Atom wm_protocols;
		Atom wm_delete_window;

	public:
		WindowManager(const OSSysEventDispatcherPtr& dispatcher, XDisplay display) :
			WindowManagerBase(dispatcher, display),
			wm_protocols(XInternAtom(display, "WM_PROTOCOLS", False)),
			wm_delete_window(XInternAtom(display, "WM_DELETE_WINDOW", False))
		{
			if (wm_protocols == None or wm_delete_window == None)
				pisk::logger::warning("window_manager", "Unable to initialize WM_PROTOCOLS X Window interface");
		}

	private:
		virtual bool handle(const OSSysEvent& event) override
		{
			if (event.type == ClientMessage)
				if (handle(event.xclient))
					return true;
			return WindowManagerBase::handle(event);
		}

		bool handle(const XClientMessageEvent& event)
		{
			if (wm_protocols != None and event.message_type == wm_protocols)
				return handle_wm_protocol(event);

			return false;
		}

	private:
		bool handle_wm_protocol(const XClientMessageEvent& event)
		{
			if (wm_delete_window != None and static_cast<XWindow>(event.data.l[0]) == wm_delete_window)
				return on_delete_window(event);

			return false;
		}

		bool on_delete_window(const XClientMessageEvent& event) threadsafe
		{
			auto window = get_window(event.window);
			if (window == nullptr)
			{
				pisk::logger::warning("window_manager", "Unknown window, handle {}, display {}", event.window, event.display);
				return false;
			}
			window->close_request.emit(window);
			return true;
		}
	};

}//namespace impl
}//namespace os
}//namespace pisk

pisk::tools::SafeComponentPtr __cdecl window_manager_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&window_manager_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto dispatcher = temp_sl.get<pisk::os::OSSysEventDispatcher>();
	if (dispatcher == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to locate SysEventDispatcher");
		throw pisk::infrastructure::NullPointerException();
	}
	auto app = temp_sl.get<pisk::os::unix_gui::OsAppInstance>();
	if (app == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to locate OsAppInstance");
		throw pisk::infrastructure::NullPointerException();
	}
	pisk::os::XDisplay display = app->get_display();
	if (display == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to connect to X DISPLAY");
		throw pisk::infrastructure::InitializeError();
	}

	return factory.make<pisk::os::impl::WindowManager>(dispatcher, display);
}

