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

#include <pisk/os/android/jni_os_headers.h>

#include "../../MainLoopBase.h"
#include "../../MainLoopSignal.h"
#include "../../MainLoopTasks.h"

namespace pisk
{
namespace os
{
namespace impl
{

using MainLoopAndroidBase = pisk::os::MainLoopSignal<pisk::os::MainLoopTasks<pisk::os::MainLoopBase>>;

class MainLoop :
	public MainLoopAndroidBase
{
public:
	template <typename ... TArgs>
	explicit MainLoop(TArgs&& ... args) :
		MainLoopAndroidBase(std::forward<TArgs>(args)...)
	{}

	virtual void stop() override
	{
		MainLoopAndroidBase::stop();
		if (auto looper = ALooper_forThread())
			ALooper_wake(looper);
	}
};

}
}
}

