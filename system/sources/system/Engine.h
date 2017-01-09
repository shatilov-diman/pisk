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

#include "EngineTask.h"

namespace pisk
{
namespace system
{
namespace impl
{
	//TODO: it is no required to known EngineTask
	class Engine :
		public system::Engine
	{
	public:
		explicit Engine(EngineTaskPtr&& _task) :
			task(std::move(_task))
		{
			if (task == nullptr)
				throw infrastructure::NullPointerException();
		}

		virtual void release() final override
		{
			infrastructure::Logger::get().debug("engine", "Destoying (0x%x)", task.get());
			delete this;
		}

	private:
		EngineTaskPtr task;
	};
}
}
}

