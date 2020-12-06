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

#include "GLDisplay.hpp"

namespace pisk
{
namespace graphic
{
	template <typename TWindowManager>
	class GLContextBase
	{
	public:
		using TWindowManagerPtr = tools::InterfacePtr<TWindowManager>;

	private:
		TWindowManagerPtr window_manager;
		const GLDisplayBase<TWindowManager>& display;

		EGLSurface surface = EGL_NO_SURFACE;
		EGLContext context = EGL_NO_CONTEXT;

	public:
		GLContextBase(const TWindowManagerPtr& win_manager_ex, const GLDisplayBase<TWindowManager>& display):
			window_manager(win_manager_ex),
			display(display)
		{}

		bool init(const os::WindowPtr& window, const std::vector<EGLint>& attributes = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE})
		{
			if (prepare_window(window) == false)
				return false;

			surface = eglCreateWindowSurface(display.get_handle(), display.get_config(), get_window_handle(window), nullptr);
			if (surface == EGL_NO_SURFACE)
			{
				pisk::logger::error("gl_window", "Unalbe to create egl surface, error: {}", egl_get_error());
				throw infrastructure::InitializeError();
			}
			context = eglCreateContext(display.get_handle(), display.get_config(), nullptr, attributes.data());
			if (context == EGL_NO_CONTEXT)
			{
				pisk::logger::error("gl_window", "Unalbe to create egl context, error: {}", egl_get_error());
				throw infrastructure::InitializeError();
			}
			return true;
		}

	protected:
		const GLDisplayBase<TWindowManager>& get_display() const
		{
			return display;
		}

		auto get_window_handle(const os::WindowPtr& window) const -> decltype(window_manager->get_handle(window))
		{
			return window_manager->get_handle(window);
		}

	private:
		virtual bool prepare_window(const os::WindowPtr& window)
		{
			(void)window;
			return true;
		}

	public:
		void deinit()
		{
			reset_current();

			if (context != EGL_NO_CONTEXT)
				if (eglDestroyContext(display.get_handle(), context) == EGL_FALSE)
					pisk::logger::error("gl_window", "Unalbe to destroy egl context, error: {}", egl_get_error());
			context = EGL_NO_CONTEXT;

			if (surface != EGL_NO_SURFACE)
				if (eglDestroySurface(display.get_handle(), surface) == EGL_FALSE)
					pisk::logger::error("gl_window", "Unalbe to destroy egl context, error: {}", egl_get_error());
			surface = EGL_NO_SURFACE;
		}

		void make_current(const os::WindowPtr&)
		{
			if (eglMakeCurrent(display.get_handle(), surface, surface, context) == EGL_FALSE)
			{
				pisk::logger::error("gl_window", "Unalbe to make gl context as current, error: {}", egl_get_error());
				throw infrastructure::InitializeError();
			}
		}

		void reset_current()
		{
			if (display.is_valid())
				if (eglMakeCurrent(display.get_handle(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE)
					pisk::logger::error("gl_window", "Unalbe to make gl context as current, error: {}", egl_get_error());
		}

		void swap_buffers()
		{
			if (display.is_valid() and surface != EGL_NO_SURFACE)
				if (eglSwapBuffers(display.get_handle(), surface) == EGL_FALSE)
					pisk::logger::error("gl_window", "Unalbe to swap buffers, error: {}", egl_get_error());
		}
	};
}//namespace graphic
}//namespace pisk

