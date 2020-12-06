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

#include <pisk/tools/OsAppInstance.h>

#include "x11.h"

namespace pisk
{
namespace os
{
namespace unix_gui
{
	class OsAppInstance :
		public tools::OsAppInstance
	{
		XDisplay display;

		virtual void release() final override
		{
			delete this;
		}

	public:
		explicit OsAppInstance(XDisplay display):
			display(display)
		{}

		XDisplay get_display() const
		{
			return display;
		}
	};
}//namespace unix_gui
}//namespace os
}//namespace pisk

