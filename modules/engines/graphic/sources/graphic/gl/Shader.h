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

#include <pisk/system/ResourceManager.h>

#include "../resources/ShaderResource.h"
#include "opengl.h"
#include "handle_ptr.h"

namespace pisk
{
namespace graphic
{
	class GLShader :
		public system::SubResource
	{
		handle_ptr<GLuint> handle;

	public:
		explicit GLShader(const ShaderSourcePtr& shader)
			: handle(glCreateShader(to_gl_shader_type(shader->get_type())), &glDeleteShader)
		{
			auto ptr = reinterpret_cast<const char*>(shader->get_source().data());
			glShaderSource(handle, 1, &ptr, nullptr);
			check_gl_error_or_die("GLShader, glShaderSource");

			glCompileShader(handle);
			check_gl_error_or_die("GLShader, glCompileShader");

			GLint is_compiled = GL_FALSE;
			glGetShaderiv(handle, GL_COMPILE_STATUS, &is_compiled);
			check_gl_error_or_die("GLShader, glGetShaderiv");
			if (is_compiled == GL_FALSE)
			{
				GLint max_length = 0;
				glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &max_length);

				std::string error(max_length, '\x0');
				glGetShaderInfoLog(handle, error.size(), &max_length, const_cast<char*>(error.data()));

				logger::error("shader", "Unable to compile shader: " + error);
				throw infrastructure::InvalidArgumentException();
			}
		}

		GLuint get_handle() const
		{
			return handle;
		}

	private:
		static GLenum to_gl_shader_type(const utils::keystring& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment")
				return GL_FRAGMENT_SHADER;

			logger::warning("shader", "Shader type '" + type.get_content() + "' is not supported");
			return GL_NONE;
		}
	};
	using GLShaderPtr = graphic::subresource_ptr<GLShader>;
	using GLShaders = std::map<utils::keystring, GLShaderPtr>;

}//namespace graphic
}//namespace pisk

