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

#include <pisk/tools/ComponentsLoader.h>

#include "Engine.h"
#include "EngineStrategy.h"

namespace pisk
{
namespace system
{
	class EngineComponentFactory :
		public core::Component
	{
	public:
		constexpr static const char* uid = "engine_component_factory";

		virtual tools::SafeComponentPtr make_engine(const tools::InstanceFactory& factory, const StrategyFactory& strategy_factory) = 0;
	};
}
}

