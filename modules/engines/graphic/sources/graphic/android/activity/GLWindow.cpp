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


#include <pisk/tools/RemoteTaskList.h>
#include <pisk/os/android/WindowManager.h>

#include "../../gl/GLContext.hpp"
#include "../../gl/GLWindow.hpp"


namespace pisk
{
namespace graphic
{
namespace impl
{
	class GLDisplay final :
		public GLDisplayBase<os::android::WindowManager>
	{
	public:
		GLDisplay(const os::android::WindowManagerPtr& win_manager_ex, const std::vector<EGLint>& attributes)
		{
			attach_display(eglGetDisplay(EGL_DEFAULT_DISPLAY));
			select_config(attributes);
		}
	};
	using GLDisplayPtr = std::unique_ptr<GLDisplay>;

	class GLContext final :
		public GLContextBase<os::android::WindowManager>
	{
	public:
		using GLContextBase<os::android::WindowManager>::GLContextBase;

	private:
		virtual bool prepare_window(const os::WindowPtr& window) override
		{
			const auto format = get_display().get_native_format();
			auto handle = get_window_handle(window);
			if (handle == nullptr)
				return false;
			ANativeWindow_setBuffersGeometry(handle, 0, 0, format);
			return true;
		}
	};
	using GLContextPtr = std::unique_ptr<GLContext>;


	class GLWindow final :
		public graphic::GLWindowBase<os::android::WindowManager, os::android::Window>
	{
		using base_t = graphic::GLWindowBase<os::android::WindowManager, os::android::Window>;

	public:

		GLWindow(const os::WindowManagerPtr& win_manager) :
			base_t(win_manager)
		{}

	private:

		virtual void request_window() final override
		{}

		virtual void subscribe_window(const os::android::WindowPtr& wnd, std::set<utils::auto_unsubscriber>& subscriptions) override
		{}

		GLDisplayPtr make_display() const
		{
			return std::make_unique<GLDisplay>(get_window_manager(), GLDisplay::get_default_display_attributes());
		}

		GLContextBasePtr make_context(const GLDisplay& display)
		{
			return std::make_unique<GLContext>(get_window_manager(), display);
		}

		virtual bool on_attach_window(const os::android::WindowPtr& wnd) override
		{
			auto display = make_display();
			auto context = make_context(*display);
			if (context->init(wnd) == false)
				return false;

			attach_display(std::move(display));
			attach_context(std::move(context));
			return true;
		}

		virtual void on_detach_window(const os::android::WindowPtr&) override
		{
			detach_context()->deinit();
			detach_display();
		}
	};

}//namespace impl

	GLWindowPtr make_gl_window(const os::WindowManagerPtr& window_manager)
	{
		auto android_win_manager = window_manager.cast<os::android::WindowManager>();
		if (android_win_manager == nullptr)
		{
			pisk::logger::error("gl_window", "Unable to get WindowManagerEx");
			throw infrastructure::LogicErrorException();
		}
		return std::make_unique<impl::GLWindow>(android_win_manager);
	}

}//namespace graphic
}//namespace pisk

