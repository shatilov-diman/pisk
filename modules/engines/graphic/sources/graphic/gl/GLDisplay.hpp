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

#include "opengl.h"

namespace pisk
{
namespace graphic
{
	template <typename TWindowManager>
	class GLDisplayBase
	{
		EGLDisplay display = EGL_NO_DISPLAY;
		EGLConfig config;

	public:
		using TWindowManagerPtr = tools::InterfacePtr<TWindowManager>;

		static std::vector<EGLint> get_default_display_attributes()
		{
			return std::initializer_list<EGLint>{
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_RED_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_BLUE_SIZE, 8,
				EGL_DEPTH_SIZE, 8,
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
				EGL_NONE
			};
		};

		GLDisplayBase()
		{
			eglBindAPI(EGL_OPENGL_ES_API);
		}

		virtual ~GLDisplayBase()
		{
			detach_display();
		}

	protected:
		void attach_display(const EGLDisplay _display)
		{
			if (_display == EGL_NO_DISPLAY)
			{
				pisk::logger::error("gl_context", "Unable to attach display, error: {}", egl_get_error());
				throw infrastructure::InitializeError();
			}
			if (display != EGL_NO_DISPLAY)
			{
				pisk::logger::critical("gl_context", "Display already attached", egl_get_error());
				throw infrastructure::InitializeError();
			}
			display = _display;
			eglInitialize(display, 0, 0);
		}

		void detach_display()
		{
			if (display != EGL_NO_DISPLAY)
				eglTerminate(display);
			display = EGL_NO_DISPLAY;
		}

		void select_config(const std::vector<EGLint>& attributes)
		{
			EGLint numConfigs{};
			if (eglChooseConfig(display, attributes.data(), &config, 1, &numConfigs) == EGL_FALSE)
			{
				pisk::logger::error("gl_context", "Unable to choose config, error: {}", egl_get_error());
				throw infrastructure::InitializeError();
			}
			if (numConfigs <= 0)
			{
				pisk::logger::error("gl_context", "Unable to choose config, error: {}, {}", egl_get_error(), numConfigs);
				throw infrastructure::InitializeError();
			}
		}

	public:
		bool is_valid() const
		{
			return  display != EGL_NO_DISPLAY;
		}

		EGLDisplay get_handle() const
		{
			return display;
		}

		EGLConfig get_config() const
		{
			return config;
		}

		EGLint get_native_format() const
		{
			EGLint format;
			if (eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format) == EGL_FALSE)
			{
				pisk::logger::error("gl_context", "Unable to get native format, error: {}", egl_get_error());
				throw infrastructure::InitializeError();
			}
			return format;
		}
	};
}//namespace graphic
}//namespace pisk

