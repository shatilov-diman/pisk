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

#include "Buffer.h"

#include <pisk/defines.h>
#include <pisk/utils/noncopyable.h>
#include <pisk/utils/keystring.h>

#include <map>

namespace pisk
{
namespace graphic
{

	class Attribute :
		public utils::noncopyable
	{
		const GLint components_count;
		const GLenum components_type;
		const GLboolean components_normalized_flag;

		GLBufferPtr buffer;

	public:
		Attribute(const GLBufferPtr& _buffer, const GLint& comp_count, const GLenum& comp_type, const GLboolean& comp_normalized_flag) :
			components_count(comp_count),
			components_type(comp_type),
			components_normalized_flag(comp_normalized_flag),
			buffer(_buffer)
		{
		}

		std::size_t get_vertex_count() const
		{
			return buffer->get_vertex_count();
		}

		GLenum get_vertex_pack_type() const
		{
			return buffer->get_vertex_pack_type();
		}


		void activate(const GLuint position)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer->get_handle());
			glVertexAttribPointer(
				position,
				components_count,
				components_type,
				components_normalized_flag,
				buffer->get_stride(),
				nullptr
			);
			glEnableVertexAttribArray(position);
		}

		void deactivate(const GLuint position)
		{
			glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
			glDisableVertexAttribArray(position);
		}
	};
	using AttributePtr = std::shared_ptr<Attribute>;
	using Attributes = std::map<utils::keystring, AttributePtr>;
}//namespace graphic
}//namespace pisk



