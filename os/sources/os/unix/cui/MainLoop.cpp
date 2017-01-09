// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module os of the project pisk.
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


#include <pisk/defines.h>
#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentsLoader.h>

#include <pisk/tools/MainLoop.h>

#include "SignalProcessor.h"
#include "KeyboardProcessor.h"

#include <chrono>
#include <thread>
#include <atomic>

namespace pisk
{
namespace os
{
namespace impl
{
	volatile std::sig_atomic_t SignalProcessor::lastsignal = 0;

	using Processor = std::function<void () noexcept>;
	using Processors = std::vector<Processor>;

	class MainLoop : public tools::MainLoop
	{
		Processors processors;
		std::atomic_bool loop;

	public:
		MainLoop():
			loop(true)
		{}

		void attach(const Processors& _processors)
		{
			std::copy(_processors.begin(), _processors.end(), std::back_inserter(processors));
		}

	private:
		virtual void run() final override
		{
			infrastructure::Logger::get().debug("loop", "Begin main loop");
			{
				while ((bool)loop)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					processes();
				}
			}
			infrastructure::Logger::get().debug("loop", "End main loop");
		}
		virtual void stop() final override
		{
			loop = false;
		}
		virtual void release() final override
		{
			delete this;
		}

		void processes()
		{
			for (const auto& processor : processors)
				processor();
		}
	};
}
}
}

pisk::tools::SafeComponentPtr __cdecl main_loop_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&main_loop_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto dispatcher = temp_sl.get<pisk::os::OSSysEventDispatcher>();
	if (dispatcher == nullptr)
	{
		pisk::infrastructure::Logger::get().critical("loop_factory", "Unable to locate SysEventDispatcher");
		throw pisk::infrastructure::NullPointerException();
	}

	auto loop = factory.make<pisk::os::impl::MainLoop>();

	pisk::os::impl::Processors processors;
	processors.emplace_back(std::bind(&pisk::os::impl::SignalProcessor::process, std::make_shared<pisk::os::impl::SignalProcessor>(*loop)));
	processors.emplace_back(std::bind(&pisk::os::impl::KeyboardProcessor::process, std::make_shared<pisk::os::impl::KeyboardProcessor>(dispatcher)));

	loop->attach(processors);

	return loop;
}

