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

#include "../SysEventDispatcher.h"

#include "jni_os_headers.h"

namespace pisk
{
namespace os
{
	struct Event
	{
		enum class Type
		{
			Unknown,
			RawEvent,
			CmdEvent,
			InputEvent,
		};
		Type type;
		struct android_app* app;

		Event(const Event::Type type, struct android_app* app):
			type(type),
			app(app)
		{}
	};
	struct RawEvent :
		public Event
	{
		struct android_poll_source* source;
		int ident;

		RawEvent(const Event::Type type, struct android_app* app, struct android_poll_source* source, const int ident):
			Event(type, app),
			source(source),
			ident(ident)
		{}
	};
	struct CmdEvent :
		public Event
	{
		int32_t cmd;

		CmdEvent(const Event::Type type, struct android_app* app, const int32_t cmd):
			Event(type, app),
			cmd(cmd)
		{}
	};
	struct InputEvent :
		public Event
	{
		struct AInputEvent* input;

		InputEvent(const Event::Type type, struct android_app* app, struct AInputEvent* input):
			Event(type, app),
			input(input)
		{}
	};

	using OSSysEvent = Event;
	using OSSysEventDispatcher = SysEventDispatcher<OSSysEvent>;
	using OSSysEventDispatcherPtr = SysEventDispatcherPtr<OSSysEvent>;
	using OSSysEventHandler = SysEventHandler<OSSysEvent>;
}
}

