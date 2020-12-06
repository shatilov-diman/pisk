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
#include <pisk/os/unix/gui/WindowManager.h>

#include "../../gl/GLContext.hpp"
#include "../../gl/GLWindow.hpp"


namespace pisk
{
namespace graphic
{
namespace impl
{
	class GLDisplay final :
		public GLDisplayBase<os::unix_gui::WindowManager>
	{
	public:
		GLDisplay(const os::unix_gui::WindowManagerPtr& win_manager_ex, const std::vector<EGLint>& attributes)
		{
			attach_display(eglGetDisplay(win_manager_ex->get_display()));
			select_config(attributes);
		}
	};
	using GLDisplayPtr = std::unique_ptr<GLDisplay>;

	class GLContext final :
		public GLContextBase<os::unix_gui::WindowManager>
	{
	public:
		using GLContextBase<os::unix_gui::WindowManager>::GLContextBase;
	};
	using GLContextPtr = std::unique_ptr<GLContext>;


	class GLWindow final :
		public graphic::GLWindowBase<os::unix_gui::WindowManager, os::unix_gui::Window>
	{
		using base_t = graphic::GLWindowBase<os::unix_gui::WindowManager, os::unix_gui::Window>;

	public:

		GLWindow(const os::WindowManagerPtr& win_manager) :
			base_t(win_manager)
		{
			attach_display(make_display());
		}

		static os::unix_gui::WindowPtr make_os_window(os::unix_gui::WindowManagerPtr window_manager, const EGLint format_id)
		{
			int num_visuals = 0;
			XVisualInfo visTemplate;
			visTemplate.visualid = format_id;

			auto vis_info_array = std::unique_ptr<XVisualInfo, int (*)(void*)>(
				XGetVisualInfo(window_manager->get_display(), VisualIDMask, &visTemplate, &num_visuals),
				&XFree
			);
			return window_manager->make_window(*vis_info_array, "pisk");
		}

	private:

		virtual void request_window() final override
		{
			const EGLint format_id = get_display().get_native_format();
			auto wnd = make_os_window(get_window_manager(), format_id);
			if (wnd == nullptr)
			{
				pisk::logger::error("gl_facade", "Unable to init GLWindow");
				throw infrastructure::LogicErrorException();
			}
		}

		virtual void subscribe_window(const os::unix_gui::WindowPtr& wnd, std::set<utils::auto_unsubscriber>& subscriptions) override
		{
			subscriptions.insert(wnd->close_request.subscribe([this](const os::WindowPtr&) {
				pisk::logger::info("gl_window", "Close window request async");
				call_from_engine_thread_async([this]() {
					pisk::logger::info("gl_window", "Close window request");
					this->on_close_request.emit();
				});
			}));
		}

		GLDisplayBasePtr make_display() const
		{
			return std::make_unique<GLDisplay>(get_window_manager(), GLDisplay::get_default_display_attributes());
		}

		GLContextBasePtr make_context(const GLDisplayBase<os::unix_gui::WindowManager>& display)
		{
			return std::make_unique<GLContext>(get_window_manager(), display);
		}

		virtual bool on_attach_window(const os::unix_gui::WindowPtr& wnd) override
		{
			auto context = make_context(get_display());
			if (context->init(wnd) == false)
				return false;

			attach_context(std::move(context));
			wnd->set_fullscreen_mode(true);
			return true;
		}

		virtual void on_detach_window(const os::unix_gui::WindowPtr& wnd) override
		{
			detach_context()->deinit();
			get_window_manager()->destroy_window(wnd);
		}
	};

}//namespace impl

	GLWindowPtr make_gl_window(const os::WindowManagerPtr& window_manager)
	{
		auto unix_win_manager = window_manager.cast<os::unix_gui::WindowManager>();
		if (unix_win_manager == nullptr)
		{
			pisk::logger::error("gl_window", "Unable to get WindowManagerEx");
			throw infrastructure::LogicErrorException();
		}
		return std::make_unique<impl::GLWindow>(unix_win_manager);
	}

}//namespace graphic
}//namespace pisk

