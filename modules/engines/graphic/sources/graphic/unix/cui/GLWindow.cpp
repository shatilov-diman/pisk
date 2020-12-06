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


#include <pisk/defines.h>
#include <pisk/utils/safequeue.h>
#include <pisk/infrastructure/Exception.h>

#include <pisk/os/WindowManager.h>

#include "../../GLWindow.h"

namespace pisk
{
namespace graphic
{
	GLWindowPtr make_gl_window(const os::WindowManagerPtr& window_manager)
	{
		throw infrastructure::UnsupportedException();
	}

}//namespace graphic
}//namespace pisk


