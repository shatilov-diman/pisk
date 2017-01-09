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


#pragma once

#include <pisk/tools/ComponentPtr.h>

#include <pisk/os/SysEvent.h>

namespace pisk
{
namespace os
{
namespace impl
{

class KeyboardProcessor
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

	void process()
	{
		int kbhit();
		int getch();

		while (kbhit())
		{
			const int key = getch();
			raise_keyboard(key, true);
			raise_keyboard(key, false);
		}
	}

private:
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

