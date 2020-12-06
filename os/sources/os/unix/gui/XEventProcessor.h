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

#include <pisk/os/unix/gui/x11.h>
#include <pisk/os/unix/gui/SysEvent.h>

namespace pisk
{
namespace os
{
namespace impl
{
	class XEventProcessor :
		public OsMessageProcessor
	{
		OSSysEventDispatcherPtr dispatcher;
		XDisplay display;

	public:
		explicit XEventProcessor(const OSSysEventDispatcherPtr& dispatcher, XDisplay display):
			dispatcher(dispatcher),
			display(display)
		{
			if (display == nullptr)
			{
				pisk::logger::critical("loop", "XDisplay not located");
				throw pisk::infrastructure::NullPointerException();
			}
		}

		virtual void loop_once() final override
		{
			if (XPending(display) == 0)
				return;

			while (XEventsQueued(display, QueuedAlready) > 0)
			{
				XEvent event;
				XNextEvent(display, &event);
				process_event(event);
			}
		}

	private:
		void process_event(const XEvent& event)
		{
			logger::spam("loop", "Event received; type: {}", event.type);
			dispatcher->dispatch(event);
		}

	};
}//namespace impl
}//namespace os
}//namespace pisk


