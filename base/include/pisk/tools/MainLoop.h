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

#include "../defines.h"
#include "../utils/noncopyable.h"
#include "../utils/signaler.h"

#include "ComponentPtr.h"

#include <functional>
#include <memory>

namespace pisk
{
namespace tools
{
	class MainLoop :
		public core::Component
	{
	public:
		constexpr static const char* uid = "main_loop";

		utils::signaler<void> on_begin_loop;
		utils::signaler<void> on_end_loop;

		void spinup()
		{
			init();
			run();
			deinit();
		}

	private:
		virtual void init() { }

		virtual void run() = 0;

		virtual void deinit() { }

	public:
		virtual void stop() = 0;
	};
	using MainLoopPtr = InterfacePtr<MainLoop>;
}
}

