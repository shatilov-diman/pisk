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

#include <pisk/os/utils.h>

#include <Windows.h>

namespace pisk
{
namespace utils
{
	EXPORT std::size_t __cdecl get_current_thread_id()
	{
		return static_cast<std::size_t>(::GetCurrentThreadId());
	}
}//namespace utils
}//namespace pisk

