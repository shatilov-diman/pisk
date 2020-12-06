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

namespace pisk
{
namespace graphic
{
	class GLWindow :
		public utils::noncopyable
	{
	public:
		utils::signaler<void> on_open;
		utils::signaler<void> on_close_request;
		utils::signaler<void> on_close;

		utils::signaler<void> on_activate;
		utils::signaler<void> on_deactivate;

		utils::signaler<void> on_update;
		utils::signaler<void> on_resize;

		virtual ~GLWindow() = default;

		virtual void request_window() = 0;

		virtual void process_delaied_tasks() = 0;

		virtual bool is_ready() const = 0;

		virtual void swap_buffers() const = 0;

		virtual os::Size get_window_size() const = 0;
	};
	using GLWindowPtr = std::unique_ptr<GLWindow>;

	GLWindowPtr make_gl_window(const os::WindowManagerPtr& window_manager);
}
}

