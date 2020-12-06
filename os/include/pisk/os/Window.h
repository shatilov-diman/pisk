
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
#include <pisk/utils/signaler.h>
#include <pisk/utils/noncopyable.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentPtr.h>
#include <pisk/tools/Releasable.h>

#include "Geometry.h"

#include <memory>

namespace pisk
{
namespace os
{
	class Window :
		public virtual tools::Releasable,
		public virtual utils::noncopyable
	{
	public:
		using WindowPtr = tools::shared_relesable_ptr<Window>;

		utils::signaler<WindowPtr> window_activate;
		utils::signaler<WindowPtr> window_deactivate;

		utils::signaler<WindowPtr> window_resize;

		virtual Size get_window_size() const threadsafe = 0;
	};
	using WindowPtr = Window::WindowPtr;

	class CloseableWindow :
		public virtual tools::Releasable,
		public virtual utils::noncopyable
	{
	public:
		utils::signaler<WindowPtr> close_request;
	};

	class TaskbarWindow :
		public virtual tools::Releasable,
		public virtual utils::noncopyable
	{
	public:
		virtual bool is_fullscreen_mode_on() const threadsafe = 0;

		virtual bool set_fullscreen_mode(const bool on) threadsafe = 0;
	};
}//namespace os
}//namespace pisk

