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

#include "opengl.h"

namespace pisk
{
namespace graphic
{
	template <typename handle_t, typename deleter_t = std::function<void (handle_t)>>
	class handle_ptr
	{
		handle_t handle;
		deleter_t deleter;

	public:
		handle_ptr(handle_t&& h, deleter_t&& d) :
			handle(std::forward<handle_t>(h)),
			deleter(std::forward<deleter_t>(d))
		{
			check_gl_error_or_die("handle_ptr, ctor");
		}
		~handle_ptr()
		{
			if (handle != handle_t{})
			{
				deleter(handle);
				check_gl_error("handle_ptr, dtor");
				handle = handle_t{};
			}
		}

		operator handle_t () const
		{
			return handle;
		}

		template <typename other_t>
		bool operator == (const other_t& o) const
		{
			return handle == o;
		}
		bool operator == (const handle_ptr& ac) const
		{
			return handle == ac.handle;
		}
	};
}//namespace graphic
}//namespace pisk

