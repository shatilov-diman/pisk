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

#include <pisk/os/unix/gui/SysEvent.h>
#include <pisk/os/unix/gui/OsAppInstance.h>

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
	pisk::os::XDisplay display;
	bool cache_keystate[255] = {};

	virtual bool handle(const os::OSSysEvent& event) final override
	{
		if (event.type == KeyPress)
		{
			if (update_state(event.xkey.keycode))
			{
				auto xkey = event.xkey;
				const auto keysym = XLookupKeysym(&xkey, 0);
				this->down.emit(keysym);
			}
			return true;
		}
		if (event.type == KeyRelease)
		{
			if (update_state(event.xkey.keycode))
			{
				auto xkey = event.xkey;
				const auto keysym = XLookupKeysym(&xkey, 0);
				this->up.emit(keysym);
			}
			return true;
		}
		return false;
	}

	bool update_state(const unsigned int keycode)
	{
		if (keycode > sizeof(cache_keystate))
		{
			logger::warning("keyboard", "Unexpected keycode: {}", keycode);
			return false;
		}
		char keys[32];
		XQueryKeymap(display, keys);
		const int index = keycode / 8;
		const int shift = keycode % 8;
		const bool pressed = (keys[index] & (1 << shift)) != 0;
		bool& cache = cache_keystate[keycode];
		if (cache == pressed)
			return false;
		cache = pressed;
		return true;
	}

public:
	Keyboard(pisk::os::XDisplay _display, const pisk::os::OSSysEventDispatcherPtr& dispatcher):
		pisk::os::OSSysEventHandler(dispatcher),
		display(_display)
	{}
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
	auto app = temp_sl.get<pisk::os::unix_gui::OsAppInstance>();
	if (app == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to locate OsAppInstance");
		throw pisk::infrastructure::NullPointerException();
	}
	pisk::os::XDisplay display = app->get_display();
	if (display == nullptr)
	{
		pisk::logger::critical("window_manager", "Unable to connect to X DISPLAY");
		throw pisk::infrastructure::InitializeError();
	}

	return factory.make<pisk::io::impl::Keyboard>(display, dispatcher);
}


extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_keyboard_factory()
{
	return &keyboard_factory;
}


