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

#include <pisk/system/EngineComponentFactory.h>
#include <pisk/system/EngineStrategy.h>

#include "Engine.h"
#include "EngineTask.h"
#include "PatchPortal.h"
#include "EngineSynchronizer.h"

namespace pisk
{
namespace system
{
namespace impl
{
	PatchPortalPtr create_patch_portal();

	class EngineComponentFactory :
		public system::EngineComponentFactory
	{
		EngineSynchronizerPtr synchronizer;
		PatchPortalPtr patch_portal;

	public:
		EngineComponentFactory():
			synchronizer(make_engine_synchronizer()),
			patch_portal(create_patch_portal())
		{
			if(synchronizer == nullptr or patch_portal == nullptr)
				throw infrastructure::NullPointerException();
		}

		void start()
		{
			synchronizer->wait_all_ready();
			synchronizer->initialize_signal();
			synchronizer->wait_all_initialized();
			synchronizer->run_loop_signal();
		}
		void stop()
		{
			synchronizer->stop_all();
			synchronizer->wait_all_loop_finished();
			synchronizer->deinitialize_signal();
			synchronizer->wait_all_deinitialized();
		}

		virtual tools::SafeComponentPtr make_engine(const tools::InstanceFactory& factory, const system::StrategyFactory& strategy_factory) final override
		{
			logger::debug("engine_factory", "New engine requested");
			if(strategy_factory == nullptr)
				throw infrastructure::NullPointerException();

			auto&& gate = patch_portal->make_gate();
			auto&& task = std::make_unique<EngineTask>(strategy_factory, synchronizer->make_slave(), std::move(gate));
			return factory.make<Engine>(std::move(task));
		}
		virtual void release() final override
		{
			logger::debug("engine_factory", "Destoying");
			delete this;
		}
	};
}
}
}

