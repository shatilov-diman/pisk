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

#include "handle_ptr.h"
#include "opengl.h"

#include <pisk/defines.h>
#include <pisk/utils/noncopyable.h>

namespace pisk
{
namespace graphic
{
	class GLBuffer :
		public utils::noncopyable
	{
		GLuint handle;
		GLsizei stride;
		std::size_t vertex_count;
		GLenum vertex_pack_type;

	public:
		GLBuffer() :
			handle{},
			stride{},
			vertex_count{},
			vertex_pack_type{}
		{
			glGenBuffers(1, &handle);
			check_gl_error_or_die("GLBuffer, glGenBuffer");
		}
		~GLBuffer()
		{
			glDeleteBuffers(1, &handle);
			check_gl_error("GLBuffer, glDeleteBuffers");
			handle = GLuint{};
		}

		GLuint get_handle() const
		{
			return handle;
		}

		void load(
			const GLenum _usage_mode,
			const infrastructure::DataBuffer& _buffer,
			const GLenum _vertex_pack_type,
			const GLsizei _stride
		)
		{
			glBindBuffer(GL_ARRAY_BUFFER, handle);
			check_gl_error_or_die("GLBuffer, glGenBuffer");

			glBufferData(GL_ARRAY_BUFFER, _buffer.size(), _buffer.data(), _usage_mode);
			check_gl_error_or_die("GLBuffer, glGenBuffer");

			vertex_count = _buffer.size() / _stride;
			stride = _stride;
			vertex_pack_type = _vertex_pack_type;
			if (vertex_count * _stride != _buffer.size())
				throw infrastructure::InvalidArgumentException();
		}

		std::size_t get_vertex_count() const
		{
			return vertex_count;
		}

		GLenum get_vertex_pack_type() const
		{
			return vertex_pack_type;
		}

		GLsizei get_stride() const
		{
			return stride;
		}
	};
	using GLBufferPtr = std::shared_ptr<GLBuffer>;
}//namespace graphic
}//namespace pisk

