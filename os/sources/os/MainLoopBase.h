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

#include <pisk/utils/sync_flag.h>
#include <pisk/tools/MainLoop.h>

#include <chrono>
#include <future>

#include "OsMessageProcessor.h"

namespace pisk
{
namespace os
{

class MainLoopBase :
	public tools::MainLoop
{
	const std::chrono::milliseconds sleep_time = std::chrono::milliseconds(10);

	OsMessageProcessorPtr os_message_processor;

	utils::sync::flag stop_requested_flag;
	utils::sync::flag ready_to_stop_flag;

public:

	explicit MainLoopBase(OsMessageProcessorPtr&& os_message_processor) :
		os_message_processor(std::move(os_message_processor))
	{}

public:
	virtual void release() final override
	{
		delete this;
	}

	virtual void init() override
	{
		logger::info("main_loop", "Begin init");
		os_message_processor->init();
		logger::info("main_loop", "End init");
	}

	virtual void run() final override
	{
		logger::info("main_loop", "Begin main loop");
		loop(std::async(std::launch::async, [this] () {
			this->on_begin_loop.emit();
		}));
		logger::info("main_loop", "Start process os events");
		loop_with_event_process(stop_requested_flag);
		logger::info("main_loop", "Stop main loop");
		loop_with_event_process(std::async(std::launch::async, [this] () {
			this->on_end_loop.remit();
		}));
		logger::info("main_loop", "Wait message processor is ready to stop");
		loop_with_event_process_wait_ready(ready_to_stop_flag);
		logger::info("main_loop", "End main loop");
	}

	virtual void deinit() override
	{
		logger::info("main_loop", "Begin deinit");
		os_message_processor->deinit();
		logger::info("main_loop", "End deinit");
	}

	virtual void stop() override
	{
		mark_request_stop();
	}

	virtual void common_loop()
	{}

private:

	void mark_request_stop()
	{
		stop_requested_flag.set();
	}
	bool is_stop_requested() const
	{
		return stop_requested_flag.get();
	}
	void mark_ready_to_stop()
	{
		ready_to_stop_flag.set();
	}
	bool is_ready_to_stop() const
	{
		return ready_to_stop_flag.get();
	}

	template <typename Waiter>
	void loop(Waiter&& waiter)
	{
		base_loop(std::forward<Waiter>(waiter), [this]() {
			common_loop();
		});
	}

	template <typename Waiter>
	void loop_with_event_process(Waiter&& waiter)
	{
		base_loop(std::forward<Waiter>(waiter), [this]() {
			process_os_messages_once();
			common_loop();
		});
	}

	template <typename Waiter>
	void loop_with_event_process_wait_ready(Waiter&& waiter)
	{
		base_loop(std::forward<Waiter>(waiter), [this]() {
			process_os_messages_once();
			common_loop();
			check_message_processor_ready_to_stop();
		});
	}
	void check_message_processor_ready_to_stop()
	{
		if (not is_ready_to_stop())
			if (os_message_processor->is_ready_to_stop())
			{
				logger::info("main_loop", "Message processor is ready to stop");
				mark_ready_to_stop();
			}
	}

	template <typename Waiter>
	void base_loop(Waiter&& waiter, std::function<void ()> loop)
	{
		while (not signaled_result(waiter.wait_for(sleep_time)))
			loop();
	}

	bool signaled_result(const bool result)
	{
		return result;
	}
	bool signaled_result(const std::future_status status)
	{
		return status == std::future_status::ready;
	}

	void process_os_messages_once()
	{
		os_message_processor->loop_once();
		if (not is_stop_requested())
			check_message_processor_request_stop();

	}
	void check_message_processor_request_stop()
	{
		if (os_message_processor->is_stop_requested())
		{
			logger::info("main_loop", "Message processor request stop");
			mark_request_stop();
		}
	}
};

}//namespace os
}//namespace pisk

