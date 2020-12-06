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

#include <pisk/utils/noncopyable.h>
#include <pisk/tools/Releasable.h>
#include <pisk/tools/ComponentsLoader.h>

namespace pisk
{
namespace system
{
	class EngineSynchronizerSlave :
		public tools::Releasable,
		public utils::noncopyable
	{
	public:

		virtual bool is_stop_requested() = 0;

		virtual void notify_ready() = 0;

		virtual void wait_initialize_signal() = 0;
		virtual void notify_initialize_finished() = 0;

		virtual void wait_loop_begin_signal() = 0;
		virtual void notify_loop_finished() = 0;

		virtual void wait_deinitialize_signal() = 0;
		virtual void notify_deinitialize_finished() = 0;
	};
	using EngineSynchronizerSlavePtr = tools::unique_relesable_ptr<EngineSynchronizerSlave>;

	class EngineSynchronizer :
		public tools::Releasable,
		public utils::noncopyable
	{
	public:
		constexpr static const char* uid = "engine_synchronizer";

		virtual EngineSynchronizerSlavePtr make_slave() = 0;

		virtual void wait_all_ready() = 0;

		virtual void initialize_signal() = 0;
		virtual void wait_all_initialized() = 0;

		virtual void run_loop_signal() = 0;
		virtual void stop_all() = 0;
		virtual void wait_all_loop_finished() = 0;

		virtual void deinitialize_signal() = 0;
		virtual void wait_all_deinitialized() = 0;
	};
	using EngineSynchronizerPtr = tools::shared_relesable_ptr<EngineSynchronizer>;

	EngineSynchronizerPtr make_engine_synchronizer();
}
}

