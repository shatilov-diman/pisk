// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module system of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional restriction according to GPLv3 pt 7:
// b) required preservation author attributions;
// c) required preservation links to original sources
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

#include <pisk/infrastructure/Logger.h>

#include <pisk/system/Engine.h>
#include <pisk/system/EngineStrategy.h>

#include "PatchPortal.h"

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
		private PatchRecipient
	{
		EngineStrategy::Configure config;

		PatchGatePtr patch_gate;
		EngineStrategyPtr strategy;

		std::atomic_bool stop;
		std::thread worker;
	public:

		EngineTask(PatchGatePtr&& _gate, const system::StrategyFactory& strategy_factory) :
			patch_gate(std::move(_gate)),
			stop(false)
		{
			infrastructure::Logger::get().debug("engine_task", "New engine task allocated (0x%x)", this);
			if (strategy_factory == nullptr)
				throw infrastructure::NullPointerException();
			strategy = strategy_factory(*this);
			if (strategy == nullptr or patch_gate == nullptr)
				throw infrastructure::NullPointerException();

			start();
		}

		~EngineTask()
		{
			infrastructure::Logger::get().debug("engine_task", "Engine task is going to stop (0x%x)", this);
			request_stop();
			wait();
			infrastructure::Logger::get().debug("engine_task", "Engine task destroied (0x%x)", this);
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
		bool is_not_stopping()
		{
			return not stop;
		}

		int run()
		{
			infrastructure::Logger::get().debug("engine_task", "Engine task starting (0x%x)", this);
			//TODO: fix on_init_app: call on event
			config = strategy->on_init_app();
			while (is_not_stopping())
			{
				wait_for_interval();
				process_input_patches();
				update();
			}
			//TODO: fix on_deinit_app: call on event
			strategy->on_deinit_app();
			infrastructure::Logger::get().debug("engine_task", "Engine task stopping (0x%x)", this);
			return 0;
		}
		void process_input_patches()
		{
			while (is_not_stopping())
			{
				PatchPtr input_patch = patch_gate->pop();
				if (input_patch == nullptr)
					break;
				strategy->patch_scene(input_patch);
			}
		}
		void update()
		{
			strategy->update();
		}
		void wait_for_interval()
		{
			std::this_thread::sleep_for(config.update_interval);
		}
	};
	using EngineTaskPtr = std::unique_ptr<EngineTask>;
}
}
}

