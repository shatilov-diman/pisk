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

#include <pisk/infrastructure/Exception.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <string>

namespace pisk
{
namespace graphic
{
	inline std::string gl_get_error(const GLenum error = glGetError())
	{
		switch (error)
		{
		case GL_NO_ERROR:
			return "No error, " + std::to_string(error);
		default:
			return "Unexpected error code, " + std::to_string(error);
		}
	}
	inline std::string egl_get_error(const EGLint error = eglGetError())
	{
		switch (error)
		{
		case EGL_SUCCESS:
			return "No error, " + std::to_string(error);
		default:
			return "Unexpected error code, " + std::to_string(error);
		}
	}

	class GLCheckException : public infrastructure::Exception
	{};

	inline bool check_gl_error(const std::string& message = "failed", const std::string& tag = "graphic")
	{
		const GLenum error = glGetError();
		if (error == GL_NO_ERROR)
			return true;
		logger::error(tag, message + ": " + gl_get_error(error));
		return false;
	}
	inline void check_gl_error_or_die(const std::string& message = "failed", const std::string& tag = "graphic")
	{
		if (check_gl_error(message, tag))
			return;
		throw GLCheckException();
	}
}
}

