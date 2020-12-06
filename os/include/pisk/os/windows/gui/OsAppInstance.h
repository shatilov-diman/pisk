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

#include "os_windows.h"

namespace pisk
{
namespace os
{
namespace windows_gui
{
	class OsAppInstance :
		public tools::OsAppInstance
	{
		HINSTANCE hinstance;

		virtual void release() final override
		{
			delete this;
		}

	public:
		explicit OsAppInstance(HINSTANCE hinstance):
			hinstance(hinstance)
		{}

		HINSTANCE get_hinstance() const
		{
			return hinstance;
		}
	};
}//namespace windows_gui
}//namespace os
}//namespace pisk

