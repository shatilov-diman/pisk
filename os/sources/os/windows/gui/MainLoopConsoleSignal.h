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

#include <atomic>

#include <Windows.h>

namespace pisk
{
namespace os
{
namespace impl
{

template <typename TMainLoopBase>
class MainLoopConsoleSignal :
	public TMainLoopBase
{
	static std::atomic_bool stop;

public:

	template <typename ... Args>
	MainLoopConsoleSignal(Args&& ... args) :
		TMainLoopBase(std::forward<Args>(args)...)
	{
		::SetConsoleCtrlHandler(&MainLoopConsoleSignal::handler, TRUE);
	}


	virtual void common_loop() override
	{
		TMainLoopBase::common_loop();
		if (stop)
		{
			TMainLoopBase::stop();
			logger::debug("console_signal", "stop signal received");
		}
	}

	private:
		static BOOL WINAPI handler(_In_ DWORD dwEventType)
		{
			pisk::logger::info("console_signal", "console signal handled, {}; stop app", dwEventType);
			stop = true;
			if (dwEventType == CTRL_CLOSE_EVENT or dwEventType == CTRL_LOGOFF_EVENT or dwEventType == CTRL_SHUTDOWN_EVENT)
			{
				static const std::size_t ms_for_exit = 10000;//It's should enough
				::Sleep(ms_for_exit);
				pisk::logger::critical("console_signal", "timeout: terminating");
			}
			return TRUE;
		}
};

}//namespace impl
}//namespace os
}//namespace pisk


