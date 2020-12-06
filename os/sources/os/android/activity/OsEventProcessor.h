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

#include <pisk/os/android/SysEvent.h>
#include <pisk/os/android/jni_os_headers.h>

namespace pisk
{
namespace os
{
namespace impl
{

class OsEventProcessor :
	public OsMessageProcessor
{
	pisk::os::OSSysEventDispatcherPtr dispatcher;
	android_app* application;

public:
	OsEventProcessor(const pisk::os::OSSysEventDispatcherPtr& dispatcher, android_app* app):
		dispatcher(dispatcher),
		application(app)
	{}

	virtual void init() final override
	{
		set_glue_handlers();
	}
	virtual void deinit() final override
	{
		reset_glue_handlers();
	}

	virtual void loop_once() final override
	{
		int ident = 0;
		int events = 0;
		struct android_poll_source* source = nullptr;

		while ((ident = ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void**>(&source))) >= 0)
		{
			logger::spam("loop", "Event received; type: {}", source->id);
			if (source != nullptr)
				source->process(application, source);

			dispatcher->dispatch( RawEvent {
				Event::Type::RawEvent,
				application,
				source,
				ident,
			});
		}
	}
	virtual bool is_stop_requested() const final override
	{
		return application->destroyRequested != 0;
	}
	virtual bool is_ready_to_stop() const final override
	{
		return application->destroyRequested != 0;
	}


private:
	void set_glue_handlers()
	{
		application->userData = this;
		application->onAppCmd = &on_app_cmd;
		application->onInputEvent = &on_input_event;
	}
	void reset_glue_handlers()
	{
		application->userData = nullptr;
	}

	static void on_app_cmd(struct android_app* app, int32_t cmd)
	{
		if (app->userData == nullptr)
			throw infrastructure::LogicErrorException();
		auto _this = static_cast<OsEventProcessor*>(app->userData);
		_this->dispatcher->dispatch( CmdEvent {
			Event::Type::CmdEvent,
			app,
			cmd
		});
	}
	static int32_t on_input_event(struct android_app* app, AInputEvent* input_event)
	{
		if (app->userData == nullptr)
			throw infrastructure::LogicErrorException();
		auto _this = static_cast<OsEventProcessor*>(app->userData);
		const bool handled = _this->dispatcher->dispatch( InputEvent {
			Event::Type::InputEvent,
			app,
			input_event
		});
		return handled ? 1 : 0;
	}
};

}
}
}

