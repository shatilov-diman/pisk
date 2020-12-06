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

#include <pisk/defines.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/os/utils.h>
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

		__android_log_print(to_android_level(level), log_tag, ('[' + std::to_string(utils::get_current_thread_id()) + "] "+ tag + ": " + message).c_str());
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

