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
			logger::debug("engine", "Destoying ({})", task.get());
			delete this;
		}

	private:
		EngineTaskPtr task;
	};
}
}
}

