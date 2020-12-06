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
#include <pisk/infrastructure/Logger.h>

#include <csignal>

namespace pisk
{
namespace os
{

template <typename TMainLoopBase>
class MainLoopSignal :
	public TMainLoopBase
{
	static std::sig_atomic_t lastsignal;

	static void signal_handler(int signal)
	{
		lastsignal = signal;
	}

public:

	template <typename ... Args>
	MainLoopSignal(Args&& ... args) :
		TMainLoopBase(std::forward<Args>(args)...)
	{
		std::signal(SIGINT, &MainLoopSignal::signal_handler);
	}

	virtual ~MainLoopSignal()
	{
		std::signal(SIGINT, SIG_DFL);
	}

	virtual void common_loop() override
	{
		TMainLoopBase::common_loop();
		if (lastsignal != 0)
		{
			lastsignal = 0;
			this->stop();
			logger::debug("main_loop", "SIGINT received: stop all");
		}
	}
};

}//namespace os
}//namespace pisk


