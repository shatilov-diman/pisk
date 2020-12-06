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

#include "../../OsMessageProcessor.h"

#include <pisk/os/windows/gui/SysEvent.h>
#include <Windows.h>

namespace pisk
{
namespace os
{
namespace impl
{
	class MSGProcessor :
		public OsMessageProcessor
	{
		OSSysEventDispatcherPtr dispatcher;
		bool stop;

	public:
		explicit MSGProcessor(const OSSysEventDispatcherPtr& _dispatcher):
			dispatcher(_dispatcher),
			stop(false)
		{
			if (dispatcher == nullptr)
				throw pisk::infrastructure::NullPointerException();
		}

		virtual void loop_once() final override
		{
			OSSysEvent msg;
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != FALSE)
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);

				if (msg.hwnd == NULL)
				{
					msg.lResult = 0;
					if (process_message(msg) == false)
						msg.lResult = ::DefWindowProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
				}

				if (msg.message == WM_QUIT)
					stop = true;
			}
		}

		virtual bool is_stop_requested() const final override
		{
			return stop;
		}

	private:
		bool process_message(const OSSysEvent& msg)
		{
			logger::spam("loop", "Message received; type: {}", msg.message);
			return dispatcher->dispatch(msg);
		}

	};
}//namespace impl
}//namespace os
}//namespace pisk



