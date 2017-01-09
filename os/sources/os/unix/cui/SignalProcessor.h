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

#include <csignal>

namespace pisk
{
namespace os
{
namespace impl
{

class SignalProcessor
{
	static volatile std::sig_atomic_t lastsignal;

	static void signal_handler(int signal)
	{
		lastsignal = signal;
	}

	tools::MainLoop& loop;

public:
	explicit SignalProcessor(tools::MainLoop& _loop):
		loop(_loop)
	{
		std::signal(SIGINT, &signal_handler);
	}
	~SignalProcessor()
	{
		std::signal(SIGINT, SIG_DFL);
	}
	void process()
	{
		if (lastsignal != 0)
		{
			loop.stop();
		}
	}
};


}
}
}

