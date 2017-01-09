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

#include <pisk/system/EngineComponentFactory.h>
#include <pisk/system/EngineStrategy.h>

#include "Engine.h"
#include "EngineTask.h"
#include "PatchPortal.h"

namespace pisk
{
namespace system
{
namespace impl
{
	class EngineComponentFactory :
		public system::EngineComponentFactory
	{
		PatchPortalPtr patch_portal;
	public:

		explicit EngineComponentFactory(PatchPortalPtr&& _portal):
			patch_portal(std::move(_portal))
		{
			if(patch_portal == nullptr)
				throw infrastructure::NullPointerException();
		}

		virtual tools::SafeComponentPtr make_engine(const tools::InstanceFactory& factory, const system::StrategyFactory& strategy_factory) final override
		{
			infrastructure::Logger::get().debug("engine_factory", "New engine requested");
			if(strategy_factory == nullptr)
				throw infrastructure::NullPointerException();

			auto&& gate = patch_portal->make_gate();
			auto&& task = std::make_unique<EngineTask>(std::move(gate), strategy_factory);
			return factory.make<Engine>(std::move(task));
		}
		virtual void release() final override
		{
			infrastructure::Logger::get().debug("engine_factory", "Destoying");
			delete this;
		}
	};
}
}
}

