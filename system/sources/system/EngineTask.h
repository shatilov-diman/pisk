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
#include <pisk/infrastructure/Logger.h>

#include <pisk/system/Engine.h>
#include <pisk/system/EngineStrategy.h>

#include "PatchPortal.h"
#include "EngineSynchronizer.h"

#include <chrono>
#include <thread>
#include <atomic>
#include <memory>

namespace pisk
{
namespace system
{
namespace impl
{
	class EngineTask :
		private PatchRecipient,
		public utils::noncopyable
	{
		EngineStrategy::Configure config;

		EngineSynchronizerSlavePtr synchronizer;
		EngineStrategyPtr strategy;
		PatchGatePtr patch_gate;

		std::chrono::system_clock::time_point last_update;
		std::atomic_bool stop;
		std::thread worker;
	public:

		EngineTask(const StrategyFactory& strategy_factory, EngineSynchronizerSlavePtr&& _synchronizer, PatchGatePtr&& _gate) :
			synchronizer(std::move(_synchronizer)),
			patch_gate(std::move(_gate)),
			stop(false)
		{
			logger::debug("engine_task", "New engine task allocated ({})", this);
			if (synchronizer == nullptr or strategy_factory == nullptr or patch_gate == nullptr)
				throw infrastructure::NullPointerException();

			strategy = strategy_factory(*this);
			if (strategy == nullptr)
				throw infrastructure::NullPointerException();

			start();
		}

		~EngineTask()
		{
			logger::debug("engine_task", "Engine task is going to stop ({})", this);
			request_stop();
			wait();
			logger::debug("engine_task", "Engine task destroied ({})", this);
		}

		virtual void push(const PatchPtr& patch) noexcept threadsafe
		{
			patch_gate->push(patch);
		}

	private:

		void start()
		{
			worker = std::thread(&EngineTask::run, this);
		}
		void wait()
		{
			worker.join();
		}
		void request_stop()
		{
			stop = true;
		}
		bool is_running()
		{
			if (synchronizer->is_stop_requested())
				return false;
			if (stop)
				return false;
			return true;
		}

		int run()
		{
			logger::debug("engine_task", "Engine task starting ({})", this);

			initialize();
			run_loop();
			deinitialize();

			logger::debug("engine_task", "Engine task stopping ({})", this);
			return 0;
		}

		void initialize()
		{
			synchronizer->notify_ready();
			synchronizer->wait_initialize_signal();
			config = strategy->on_init_app();
			synchronizer->notify_initialize_finished();
		}
		void run_loop()
		{
			synchronizer->wait_loop_begin_signal();
			while (is_running())
			{
				wait_for_interval();
				prepatch();
				process_input_patches();
				update();
			}
			synchronizer->notify_loop_finished();
		}
		void deinitialize()
		{
			synchronizer->wait_deinitialize_signal();
			strategy->on_deinit_app();
			synchronizer->notify_deinitialize_finished();
		}

		void process_input_patches()
		{
			std::deque<PatchPtr> patches;
			while (PatchPtr input_patch = patch_gate->pop())
				patches.push_back(input_patch);
			for (auto&& input_patch : patches)
				strategy->patch_scene(input_patch);
		}
		void prepatch()
		{
			strategy->prepatch();
		}
		void update()
		{
			strategy->update();
		}
		void wait_for_interval()
		{
			const auto now = std::chrono::system_clock::now();
			const auto work_time = now - last_update;
			if (work_time < config.update_interval)
				std::this_thread::sleep_for(config.update_interval - work_time);
			last_update = now;

		}
	};
	using EngineTaskPtr = std::unique_ptr<EngineTask>;
}
}
}

