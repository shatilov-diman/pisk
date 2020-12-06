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
#include <pisk/os/windows/gui/WindowManager.h>

#include "../../gl/GLContext.hpp"
#include "../../gl/GLWindow.hpp"


namespace pisk
{
namespace graphic
{
namespace impl
{
	class GLDisplay final :
		public GLDisplayBase<os::windows_gui::WindowManager>
	{
	public:
		GLDisplay(const HWND& hwnd, const std::vector<EGLint>& attributes)
		{
			if (hwnd == NULL)
			{
				pisk::logger::error("gl_context", "Unable to locate window's handle");
				throw infrastructure::InitializeError();
			}
			attach_display(eglGetDisplay(::GetDC(hwnd)));
			select_config(attributes);
		}
	};
	using GLDisplayPtr = std::unique_ptr<GLDisplay>;

	class GLContext final :
		public GLContextBase<os::windows_gui::WindowManager>
	{
	public:
		using GLContextBase<os::windows_gui::WindowManager>::GLContextBase;
	};
	using GLContextPtr = std::unique_ptr<GLContext>;

	class GLWindow final :
		public graphic::GLWindowBase<os::windows_gui::WindowManager, os::windows_gui::Window>
	{
		using base_t = graphic::GLWindowBase<os::windows_gui::WindowManager, os::windows_gui::Window>;

	public:

		using base_t::base_t;

		virtual void request_window() override
		{
			get_window_manager()->make_window("pisk");
		}

	private:

		virtual void subscribe_window(const os::windows_gui::WindowPtr& wnd, std::set<utils::auto_unsubscriber>& subscriptions) override
		{
			subscriptions.insert(wnd->close_request.subscribe([this](const os::WindowPtr&) {
				pisk::logger::info("gl_window", "Close window request async");
				call_from_engine_thread_async([this]() {
					pisk::logger::info("gl_window", "Close window request");
					this->on_close_request.emit();
				});
			}));
		}

		GLDisplayPtr make_display(const os::windows_gui::WindowPtr& wnd) const
		{
			return std::make_unique<GLDisplay>(get_window_manager()->get_handle(wnd), GLDisplay::get_default_display_attributes());
		}

		GLContextPtr make_context(const GLDisplay& _display)
		{
			return std::make_unique<GLContext>(get_window_manager(), _display);
		}

		virtual bool on_attach_window(const os::windows_gui::WindowPtr& wnd) override
		{
			auto lcl_display = make_display(wnd);
			auto lcl_context = make_context(*lcl_display);
			if (lcl_context->init(wnd) == false)
				return false;

			attach_display(std::move(lcl_display));
			attach_context(std::move(lcl_context));
			wnd->set_fullscreen_mode(true);
			return true;
		}

		virtual void on_detach_window(const os::windows_gui::WindowPtr& wnd) override
		{
			detach_context()->deinit();
			get_window_manager()->destroy_window(wnd);
			detach_display();
		}
	};

}//namespace impl

	GLWindowPtr make_gl_window(const os::WindowManagerPtr& window_manager)
	{
		auto win_manager = window_manager.cast<os::windows_gui::WindowManager>();
		if (win_manager == nullptr)
		{
			pisk::logger::error("gl_facade", "Unable to get WindowManagerEx");
			throw infrastructure::LogicErrorException();
		}
		return std::make_unique<impl::GLWindow>(win_manager);
	}

}//namespace graphic
}//namespace pisk

