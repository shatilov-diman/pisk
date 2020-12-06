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


#include <pisk/defines.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentsLoader.h>

#include <pisk/os/macos/cui/SysEvent.h>

#include "../../Keyboard.h"

namespace pisk
{
namespace io
{
namespace impl
{

class Keyboard :
	public io::Keyboard,
	private os::OSSysEventHandler
{
	virtual bool handle(const os::Event& event) final override
	{
		if (event.type == os::Event::Type::Keyboard)
		{
			const int key = static_cast<int>(event.param1);
			const bool press = static_cast<bool>(event.param2);
			if (press)
				this->down.emit(key);
			else
				this->up.emit(key);
			return true;
		}
		return false;
	}
public:
	explicit Keyboard(const pisk::os::OSSysEventDispatcherPtr& dispatcher):
		pisk::os::OSSysEventHandler(dispatcher)
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
		pisk::logger::critical("loop_factory", "Unable to locate SysEventDispatcher");
		throw pisk::infrastructure::NullPointerException();
	}

	return factory.make<pisk::io::impl::Keyboard>(dispatcher);
}


extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_keyboard_factory()
{
	return &keyboard_factory;
}


