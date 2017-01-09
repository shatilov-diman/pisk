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

#include <pisk/defines.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/os/android/jni_os_headers.h>

namespace pisk
{
namespace os
{
namespace impl
{

class AndroidConsoleLogger:
	public pisk::infrastructure::LogStorage
{
	virtual void store(const pisk::infrastructure::Logger::Level level, const std::string& tag, const std::vector<std::string>& messages) noexcept final override
	{
		for (const auto& message : messages)
			store(level, tag, message);
	}

	virtual void store(const pisk::infrastructure::Logger::Level level, const std::string& tag, const std::string& message) noexcept final override
	{
		static const char log_tag[] = "com.sample.hello_android";

		__android_log_print(to_android_level(level), log_tag, (tag + ": " + message).c_str());
	}

	static int to_android_level(const pisk::infrastructure::Logger::Level level)
	{
		using Level = pisk::infrastructure::Logger::Level;
		switch (level)
		{
			case Level::Silent:      return ANDROID_LOG_SILENT;
			case Level::Critical:    return ANDROID_LOG_FATAL;
			case Level::Error:       return ANDROID_LOG_ERROR;
			case Level::Warning:     return ANDROID_LOG_WARN;
			case Level::Information: return ANDROID_LOG_INFO;
			case Level::Debug:       return ANDROID_LOG_DEBUG;
			case Level::Spam:        return ANDROID_LOG_VERBOSE;
		};
		throw pisk::infrastructure::InvalidArgumentException();
	}
};

}
}
}

