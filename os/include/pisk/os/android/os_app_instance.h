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

#include "jni_os_headers.h"

namespace pisk
{
namespace os
{
namespace android
{
	class OsAppInstance :
		public tools::OsAppInstance
	{
		android_app* application;

		virtual void release() final override
		{
			delete this;
		}

	public:
		explicit OsAppInstance(android_app* application):
			application(application)
		{}

		android_app* get_app_instance() const
		{
			return application;
		}
	};
}//namespace android
}//namespace os
}//namespace pisk

