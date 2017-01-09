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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

namespace pisk
{
namespace os
{
namespace impl
{
	static bool oneshot;
	static struct termios orig_termios;

	void reset_terminal_mode()
	{
		oneshot = false;
		tcsetattr(0, TCSANOW, &orig_termios);
	}

	void set_conio_terminal_mode()
	{
		if (oneshot)
			throw infrastructure::LogicErrorException();
		oneshot = true;
		struct termios new_termios;

		/* take two copies - one for now, one for later */
		tcgetattr(0, &orig_termios);
		memcpy(&new_termios, &orig_termios, sizeof(new_termios));

		/* register cleanup handler, and set the new terminal mode */
		atexit(reset_terminal_mode);
		cfmakeraw(&new_termios);
		new_termios.c_oflag = orig_termios.c_oflag;
		new_termios.c_lflag |= ISIG;
		tcsetattr(0, TCSANOW, &new_termios);
	}

	int kbhit()
	{
		struct timeval tv = { 0L, 0L };
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(0, &fds);
		return select(1, &fds, NULL, NULL, &tv);
	}

	int getch()
	{
		int r;
		unsigned char c;
		if ((r = read(0, &c, sizeof(c))) < 0) {
			return r;
		} else {
			return c;
		}
	}
}
}
}

