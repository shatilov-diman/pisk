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

#include "../../Mouse.h"

namespace pisk
{
namespace io
{
namespace impl
{

class Mouse :
	public io::Mouse,
	private os::OSSysEventHandler
{
	pisk::os::XDisplay display;
	bool cache_keystate[10] = {};
	int cscreen{};
	int cx{};
	int cy{};

	virtual bool handle(const os::OSSysEvent& event) final override
	{
		if (event.type == ButtonPress)
		{
			const auto button = get_button_type(event.xbutton.button);
			if (button != ButtonType::Unknown)
				this->down.emit(button);
			else if (event.xbutton.button == Button4)
				this->wheel.emit(WheelSide::Up);
			else if (event.xbutton.button == Button5)
				this->wheel.emit(WheelSide::Down);
			return true;
		}
		if (event.type == ButtonRelease)
		{
			const auto button = get_button_type(event.xbutton.button);
			if (button != ButtonType::Unknown)
				this->up.emit(button);
			return true;
		}
		if (event.type == MotionNotify)
		{
			if (event.xmotion.x_root != cx or event.xmotion.y_root != cy)
			{
				reset_cursor_position();
				this->move.emit({event.xmotion.x_root - cx, event.xmotion.y_root - cy});
			}
			return true;
		}
		return false;
	}

	static ButtonType get_button_type(const int keycode)
	{
		switch (keycode)
		{
		case Button1: return ButtonType::Left;
		case Button2: return ButtonType::Middle;
		case Button3: return ButtonType::Right;
		default: return ButtonType::Unknown;
		}
	}

public:
	Mouse(pisk::os::XDisplay _display, const pisk::os::OSSysEventDispatcherPtr& dispatcher):
		pisk::os::OSSysEventHandler(dispatcher),
		display(_display)
	{
		if (get_center(cscreen, cx, cy) == false)
		{
			pisk::logger::error("io", "Mouse pointer not found");
			throw infrastructure::UnsupportedException();
		}
		reset_cursor_position();
		pisk::logger::debug("io", "Mouse center: ({}, {})", cx, cy);
	}

private:
	void reset_cursor_position()
	{
		Window root_window = XRootWindow(display, cscreen);
		XWarpPointer(display, None, root_window, 0, 0, 0, 0, cx, cy);
	}

	bool get_center(int& screen, int& x, int& y)
	{
		for (auto i = XScreenCount(display); i > 0; --i)
		{
			unsigned int mask{};
			int root_x{}, root_y{}, w_x{}, w_y{};

			Window window_returned{};
			Window root_window = XRootWindow(display, i-1);
			if (XQueryPointer(display, root_window, &window_returned, &window_returned, &root_x, &root_y, &w_x, &w_y, &mask) == True)
			{
				XWindowAttributes ra;
				XGetWindowAttributes(display, root_window, &ra);
				x = ra.width / 2;
				y = ra.height / 2;
				return true;
			}
		}
		return false;
	}
};

}
}
}


using namespace pisk::tools;

SafeComponentPtr __cdecl mouse_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&mouse_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

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

	return factory.make<pisk::io::impl::Mouse>(display, dispatcher);
}


extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_mouse_factory()
{
	return &mouse_factory;
}


