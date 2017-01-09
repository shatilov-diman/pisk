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

#include <Windows.h>
#include <atomic>

namespace pisk
{
namespace os
{
namespace impl
{
	class MainLoop : public tools::MainLoop
	{
		std::atomic_bool loop;

	public:
		MainLoop():
			loop(true)
		{}

	private:
		virtual void run() final override
		{
			infrastructure::Logger::get().info("app", "Begin main loop");
			{
				while ((bool)loop)
				{
					MSG msg;
					::GetMessage(&msg, NULL, 0, 0);
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			infrastructure::Logger::get().info("app", "End main loop");
		}
		virtual void stop() final override
		{
			loop = false;
		}
		virtual void release() final override
		{
			delete this;
		}
	};
}
}
}

pisk::tools::SafeComponentPtr __cdecl main_loop_factory(const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(static_cast<pisk::tools::components::ComponentFactory>(&main_loop_factory), "Signature was changed!");

	return factory.make<pisk::os::impl::MainLoop>();
}

