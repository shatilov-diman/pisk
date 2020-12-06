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
#include <pisk/utils/noncopyable.h>

#include "Shader.h"
#include "handle_ptr.h"

namespace pisk
{
namespace graphic
{
	class Program :
		public system::SubResource
	{
		GLShaders shaders;
		handle_ptr<GLuint> handle;

	public:
		Program(GLShaders&& _shaders)
			: shaders(_shaders)
			, handle(glCreateProgram(), &glDeleteProgram)
		{
			for (const auto& shader : shaders)
				glAttachShader(handle, shader.second->get_handle());
			glLinkProgram(handle);

			GLint is_linked = GL_FALSE;
			glGetProgramiv(handle, GL_LINK_STATUS, &is_linked);
			for (const auto& shader : shaders)
				glDetachShader(handle, shader.second->get_handle());

			if (is_linked == GL_FALSE)
			{
				GLint max_length = 0;
				glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &max_length);

				std::string error(max_length, '\x0');
				glGetProgramInfoLog(handle, error.size(), &max_length, const_cast<char*>(error.data()));

				logger::error("shader", "Unable to link program: " + error);
				throw infrastructure::InvalidArgumentException();
			}
		}

		GLuint get_handle() const
		{
			return handle;
		}

		void activate()
		{
			glUseProgram(handle);
		}

		void deactivate()
		{
			glUseProgram(0);
		}
	};
	using ProgramPtr = graphic::subresource_ptr<Program>;
}//namespace graphic
}//namespace pisk


