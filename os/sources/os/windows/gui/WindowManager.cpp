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
#include <pisk/tools/MainLoop.h>
#include <pisk/tools/ComponentsLoader.h>
#include <pisk/utils/sync_flag.h>

#include <pisk/os/windows/gui/OsAppInstance.h>
#include <pisk/os/windows/gui/WindowManager.h>

#include "Window.h"

#include <mutex>
#include <map>

namespace pisk
{
namespace os
{
namespace impl
{
	class WindowManager :
		public os::windows_gui::WindowManager,
		private os::OSSysEventHandler
	{
		constexpr static const char* wnd_class_name = "charivari.ltd:pisk window";
		HINSTANCE hInstance;
		ATOM wnd_class;

		mutable std::mutex mutex;
		std::map<HWND, WindowPtr> windows;

		utils::sync::flag stop_add_new_windows_flag;
		utils::sync::flag all_windows_are_closed;

	public:
		explicit WindowManager(const OSSysEventDispatcherPtr& dispatcher, HINSTANCE hInstance) :
			os::OSSysEventHandler(dispatcher),
			hInstance(hInstance),
			wnd_class(register_wnd_class(hInstance))
		{}

		void on_end_loop()
		{
			pisk::logger::info("window_manager", "end loop received");
			stop_add_new_windows();
			close_all_windows();
		}

	private:
		static ATOM register_wnd_class(HINSTANCE hInstance)
		{
			if (hInstance == NULL)
				throw infrastructure::NullPointerException();

			WNDCLASSEX wnd_class_desc = {
				sizeof(WNDCLASSEX),
				CS_DBLCLKS | CS_OWNDC,
				&WindowManager::wnd_procedure_raw,
				0, 0,
				hInstance,
				NULL,
				NULL,
				NULL,
				NULL,
				wnd_class_name,
				NULL
			};
			return ::RegisterClassEx(&wnd_class_desc);
		}

		static LRESULT CALLBACK wnd_procedure_raw(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			if (msg == WM_NCCREATE)
			{
				WindowManager* pthis = reinterpret_cast<WindowManager*>(reinterpret_cast<LPCREATESTRUCT>(lparam)->lpCreateParams);
				::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(pthis));
				pthis->add_window(tools::make_shared_releasable<impl::Window>(pthis->get_dispatcher(), hwnd));
			}
			WindowManager* _this = reinterpret_cast<WindowManager*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if (_this == nullptr)
				return ::DefWindowProc(hwnd, msg, wparam, lparam);

			const LRESULT lresult = _this->process_message(hwnd, msg, wparam, lparam);
			if (msg == WM_NCDESTROY)
				_this->remove_window(hwnd);
			return lresult;
		}

		LRESULT process_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			logger::spam("loop", "Message received; hwnd: {}, type: {}", hwnd, msg);
			OSSysEvent message;
			message.hwnd = hwnd;
			message.message = msg;
			message.wParam = wparam;
			message.lParam = lparam;
			if (get_dispatcher()->dispatch(message))
				return message.lResult;

			return ::DefWindowProc(hwnd, msg, wparam, lparam);
		}

	private://tools::Releasable

		virtual void release() final override
		{
			pisk::logger::debug("window_manager", "WindowManager is destroying");
			delete this;
			pisk::logger::debug("window_manager", "WindowManager was destroyed");
		}

	protected://os::OSSysEventHandler
		virtual bool handle(const OSSysEvent& msg) override
		{
			pisk::logger::debug("window_manager", "Window msg: {}, {}", msg.message, msg.hwnd);
			if (msg.hwnd == NULL)
				return false;
			auto window = get_window(msg.hwnd);
			if (window == nullptr)
			{
				pisk::logger::warning("window_manager", "Unknown window, handle {}", msg.hwnd);
				return false;
			}

			switch (msg.message)
			{
			case WM_CREATE:
				on_create_window(window, msg);
				return false;
			case WM_CLOSE:
				on_close_window(window, msg);
				return true;
			case WM_DESTROY:
				on_destroy_window(window, msg);
				return false;
			case WM_SIZE:
				on_resize_window(window, msg);
				return false;
			case WM_ACTIVATE:
				if (LOWORD(msg.wParam) == WA_INACTIVE)
					on_deactivate_window(window, msg);
				else
					on_activate_window(window, msg);
				return false;
			};
			return false;
		}

		void on_create_window(const WindowPtr& window, const MSG&) threadsafe
		{
			this->window_create.emit(window);
			window->window_activate.emit(window);
		}

		void on_destroy_window(const WindowPtr& window, const MSG&) threadsafe
		{
			window->window_deactivate.emit(window);
			this->window_destroy.emit(window);
			window->on_close_handle();
		}

		void on_close_window(const WindowPtr& window, const MSG&)
		{
			window->close_request.emit(window);
		}

		void on_activate_window(const WindowPtr& window, const OSSysEvent&) threadsafe
		{
			window->window_activate.emit(window);
		}
		void on_deactivate_window(const WindowPtr& window, const OSSysEvent&) threadsafe
		{
			window->window_deactivate.emit(window);
		}

		void on_resize_window(const WindowPtr& window, const OSSysEvent&) threadsafe
		{
			window->window_resize.emit(window);
		}

	private:
		void add_window(const WindowPtr& window) threadsafe
		{
			std::unique_lock<std::mutex> guard(mutex);
			if (stop_add_new_windows_flag.get())
				pisk::logger::warning("window_manager", "unable to add new window: end of loop received, handle: {}", get_handle(window));
			else
			{
				pisk::logger::debug("window_manager", "add new window, handle: {}", get_handle(window));
				windows[get_handle(window)] = window;
			}
		}

		void remove_window(const HWND window) threadsafe
		{
			std::unique_lock<std::mutex> guard(mutex);
			if (windows.erase(window) == 0)
				pisk::logger::warning("window_manager", "unable to remove window, handle: {}", window);
			else
				pisk::logger::debug("window_manager", "remove window, handle: {}", window);

			if (windows.empty())
				if (stop_add_new_windows_flag.get())
					all_windows_are_closed.set();
		}

		void stop_add_new_windows()
		{
			stop_add_new_windows_flag.set();
		}

		void close_all_windows()
		{
			const auto& all_windows = get_all_windows();
			for (const auto& pair : all_windows)
				pair.second->close_handle();

			if (not all_windows.empty())
				all_windows_are_closed.wait();
		}

		void clear()
		{
			std::unique_lock<std::mutex> guard(mutex);
			windows.clear();
		}

		std::map<HWND, WindowPtr> get_all_windows()
		{
			std::unique_lock<std::mutex> guard(mutex);
			return windows;
		}

	protected:
		WindowPtr get_window(const HWND window) threadsafe const
		{
			std::unique_lock<std::mutex> guard(mutex);
			auto found = windows.find(window);
			if (found == windows.end())
				return nullptr;
			return found->second;
		}

	private://os::windows_gui::WindowManager
		virtual HINSTANCE get_instance() const threadsafe final override
		{
			return hInstance;
		}

		static Size get_screen_size() threadsafe
		{
			const int width = GetSystemMetrics(SM_CXSCREEN);
			const int height = GetSystemMetrics(SM_CYSCREEN);
			return {static_cast<unsigned int>(width), static_cast<unsigned int>(height)};
		}

		static HWND make(HINSTANCE hInstance, const utils::keystring& class_name, const utils::keystring& name, LPVOID param)
		{
			const auto screen_size = get_screen_size();

			HWND hwnd = ::CreateWindowEx(
				0,
				class_name.c_str(),
				name.c_str(),
				WS_VISIBLE,
				0, 0, screen_size.cx, screen_size.cy,
				NULL,
				NULL,
				hInstance,
				param
			);
			if (hwnd == 0)
				pisk::logger::error("window_manager", "Unable to create Window");
			else
				pisk::logger::debug("window_manager", "Window made: handle: {}; width: {}; height: {};", hwnd, screen_size.cx, screen_size.cy);

			return hwnd;
		}
		virtual void make_window(const utils::keystring& name, const on_window_create_success_t& on_success, const on_window_create_failed_t& on_failed) threadsafe final override
		{
			run_from_dispatcher_thread_async([this, name, on_success, on_failed]() {
				HWND hwnd = make(get_instance(), wnd_class_name, name, this);
				auto wnd = hwnd == NULL ? nullptr : get_window(hwnd);
				if (wnd == NULL)
				{
					if (on_failed != nullptr)
						on_failed();
				}
				else
				{
					if (on_success != nullptr)
						on_success(wnd);
				}
			});
		}

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
			run_from_dispatcher_thread_async([this, window]() {
				window->close_handle();
			});
			return true;
		}

		virtual HWND get_handle(const os::WindowPtr& wnd) const threadsafe final override
		{
			WindowPtr window = std::dynamic_pointer_cast<Window>(wnd);
			if (window == nullptr)
			{
				pisk::logger::error("window_manager", "get_handle: unable to get internal interface from Window object {}", wnd.get());
				return 0;
			}
			return window->get_handle();
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
	auto main_loop = temp_sl.get<pisk::tools::MainLoop>();
	if (main_loop == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to locate MainLoop");
		return {};
	}
	auto app = temp_sl.get<pisk::os::windows_gui::OsAppInstance>();
	if (app == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to locate OsAppInstance");
		throw pisk::infrastructure::NullPointerException();
	}
	HINSTANCE hInstance = app->get_hinstance();
	if (hInstance == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to get HINSTANCE");
		throw pisk::infrastructure::InitializeError();
	}

	auto manager = factory.make<pisk::utils::subscribtions_holder_proxy<pisk::os::impl::WindowManager>>(dispatcher, hInstance);
	manager->store_subscribtion(main_loop->on_end_loop.subscribe([raw_manager = manager.get()]() {//use raw pointer to avoid issue with cyclic links
		raw_manager->on_end_loop();
	}));
	return manager;
}

