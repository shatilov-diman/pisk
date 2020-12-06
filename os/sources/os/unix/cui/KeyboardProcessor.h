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

#include <pisk/tools/ComponentPtr.h>

#include "../../OsMessageProcessor.h"

#include <pisk/os/unix/cui/SysEvent.h>

namespace pisk
{
namespace os
{
namespace impl
{

class KeyboardProcessor :
	public OsMessageProcessor
{
	tools::InterfacePtr<pisk::os::OSSysEventDispatcher> dispatcher;

public:
	explicit KeyboardProcessor(const tools::InterfacePtr<pisk::os::OSSysEventDispatcher>& _dispatcher):
		dispatcher(_dispatcher)
	{
		if (dispatcher == nullptr)
			throw infrastructure::NullPointerException();
		void set_conio_terminal_mode();
		set_conio_terminal_mode();
	}
	~KeyboardProcessor()
	{
		void reset_terminal_mode();
		reset_terminal_mode();
	}

	virtual void loop_once() final override
	{
		while (kbhit())
		{
			const int key = getch();
			raise_keyboard(key, true);
			raise_keyboard(key, false);
		}
	}

private:
	static int kbhit();
	static int getch();

	void raise_keyboard(const int key, const bool press)
	{
		dispatcher->dispatch({
			Event::Type::Keyboard,
			static_cast<std::size_t>(key),
			static_cast<std::size_t>(press)
		});
	}
};

}
}
}

