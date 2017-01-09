// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module io of the project pisk.
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

#include <pisk/os/SysEvent.h>

#include "../../Keyboard.h"

namespace pisk
{
namespace io
{
namespace impl
{

class Keyboard :
	public io::Keyboard,
	private os::SysEventHandler<os::Event>
{
	virtual void handle(const os::Event& event) final override
	{
		if (event.type == os::Event::Type::Keyboard)
		{
			const int key = static_cast<int>(event.param1);
			const bool press = static_cast<bool>(event.param2);
			if (press)
				this->down.emit(key);
			else
				this->up.emit(key);
		}
	}
public:
	explicit Keyboard(const pisk::os::SysEventDispatcherPtr<os::Event>& dispatcher):
		pisk::os::SysEventHandler<os::Event>(dispatcher)
	{
	}
};

}
}
}


using namespace pisk::tools;

SafeComponentPtr __cdecl keyboard_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&keyboard_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto dispatcher = temp_sl.get<pisk::os::OSSysEventDispatcher>();
	if (dispatcher == nullptr)
	{
		pisk::infrastructure::Logger::get().critical("loop_factory", "Unable to locate SysEventDispatcher");
		throw pisk::infrastructure::NullPointerException();
	}

	return factory.make<pisk::io::impl::Keyboard>(dispatcher);
}


extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_keyboard_factory()
{
	return &keyboard_factory;
}

