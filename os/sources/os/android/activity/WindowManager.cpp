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
#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentsLoader.h>

#include <pisk/os/android/jni_os_headers.h>
#include <pisk/os/android/os_app_instance.h>
#include <pisk/os/android/SysEvent.h>
#include <pisk/os/android/WindowManager.h>

#include <atomic>
#include <mutex>
#include <map>

namespace pisk
{
namespace os
{
namespace impl
{
	class Window final :
		public android::Window
	{
		std::atomic<ANativeWindow*> handle;

		explicit Window(ANativeWindow* h):
			handle(h)
		{
			if (handle == nullptr)
				throw infrastructure::NullPointerException();
			ANativeWindow_acquire(handle);
			pisk::logger::debug("window_manager", "Window {} was made, handle {}", this, handle.load());
		}

		virtual ~Window()
		{
			pisk::logger::debug("window_manager", "Window {} was destroied, handle {}", this, handle.load());
			ANativeWindow_release(handle);
		}

	public:
		using WindowPtr = std::shared_ptr<Window>;

		static WindowPtr make(ANativeWindow* handle) threadsafe
		{
			if (handle == nullptr)
			{
				pisk::logger::error("window_manager", "Unable to locate app->window");
				return {};
			}
			return tools::make_shared_releasable_raw<Window>(new Window(handle));
		}

		ANativeWindow* get_handle() const threadsafe
		{
			return handle;
		}

	public://Releasable
		virtual void release() final override
		{
			delete this;
		}


	public://os::Window
		virtual Size get_window_size() const threadsafe final override
		{
			const int width = ANativeWindow_getWidth(get_handle());
			const int height = ANativeWindow_getHeight(get_handle());
			return {static_cast<unsigned int>(width), static_cast<unsigned int>(height)};
		}
	};
	using WindowPtr = Window::WindowPtr;


	class WindowManager final :
		public os::android::WindowManager,
		private os::OSSysEventHandler
	{
		android_app* application;
		WindowPtr window;

	public:
		WindowManager(const pisk::os::OSSysEventDispatcherPtr& dispatcher, android_app* app):
			os::OSSysEventHandler(dispatcher),
			application(app)
		{
			if (application == nullptr)
				throw infrastructure::NullPointerException();
		}

	private://tools::Releasable
		virtual void release() final override
		{
			pisk::logger::debug("window_manager", "WindowManager is destroying");
			delete this;
			pisk::logger::debug("window_manager", "WindowManager was destroyed");
		}

	private://os::OSSysEventHandler
		virtual bool handle(const OSSysEvent& event) final override
		{
			switch (event.type)
			{
				case Event::Type::CmdEvent:
					return handle(static_cast<const CmdEvent&>(event));

				default:;
			};
			return false;
		}

		bool handle(const CmdEvent& event)
		{
			pisk::logger::info("window_manager", "CmdEvent: {}", event.cmd);
			switch (event.cmd)
			{
				case APP_CMD_INIT_WINDOW:
					on_init_window(event);
					return true;
				case APP_CMD_TERM_WINDOW:
					on_term_window();
					return true;
				case APP_CMD_GAINED_FOCUS:
					on_activate_window();
					return true;
				case APP_CMD_LOST_FOCUS:
					on_deactivate_window();
					return true;

				default:;
			};
			return false;
		}

		void on_init_window(const CmdEvent& event)
		{
			pisk::logger::debug("window_manager", "on_init_window");
			if (window != nullptr)
			{
				pisk::logger::warning("window_manager", "on_init_window: already made: destroy it!");
				on_term_window();
			}
			auto wnd = Window::make(event.app->window);
			if (wnd == nullptr)
			{
				pisk::logger::warning("window_manager", "on_init_window: unable to made window");
				return;
			}
			window = wnd;
			this->window_create.emit(window);
		}

		void on_term_window()
		{
			pisk::logger::debug("window_manager", "on_term_window");
			if (window == nullptr)
			{
				pisk::logger::warning("window_manager", "on_term_window: window does not exist");
				return;
			}
			this->window_destroy.emit(window);
			window.reset();
		}

		void on_activate_window()
		{
			pisk::logger::debug("window_manager", "on_activate_window");
			if (window == nullptr)
				pisk::logger::warning("window_manager", "on_activate_window: window does not exist");
			else
				window->window_activate.emit(window);
		}

		void on_deactivate_window()
		{
			pisk::logger::debug("window_manager", "on_deactivate_window");
			if (window == nullptr)
				pisk::logger::warning("window_manager", "on_deactivate_window: window does not exist");
			else
				window->window_deactivate.emit(window);
		}

	private://os::WindowManager

		virtual std::set<os::WindowPtr> get_windows() threadsafe const final override
		{
			return { window };
		}

	private://os::OSSysEventHandler
		virtual struct ANativeWindow* get_handle(const os::WindowPtr& wnd) threadsafe const final override
		{
			WindowPtr impl_wnd = std::dynamic_pointer_cast<Window>(wnd);
			if (impl_wnd == nullptr)
			{
				pisk::logger::error("window_manager", "get_handle: unable to get internal interface from Window object {}", wnd.get());
				return nullptr;
			}
			return impl_wnd->get_handle();
		}
	};
}
}
}

pisk::tools::SafeComponentPtr __cdecl window_manager_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&window_manager_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto dispatcher = temp_sl.get<pisk::os::OSSysEventDispatcher>();
	if (dispatcher == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to locate SysEventDispatcher");
		throw pisk::infrastructure::NullPointerException();
	}
	auto app = temp_sl.get<pisk::os::android::OsAppInstance>();
	if (app == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to locate OsAppInstance");
		throw pisk::infrastructure::NullPointerException();
	}

	return factory.make<pisk::os::impl::WindowManager>(dispatcher, app->get_app_instance());
}

