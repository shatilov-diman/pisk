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

#include <iostream>

namespace pisk
{
namespace os
{
namespace impl
{

class ConsoleLogger:
	public infrastructure::LogStorage
{
	virtual void store(const infrastructure::Logger::Level level, const std::string& tag, const std::string& message) noexcept final override
	{
		const std::string tid = '[' + std::to_string(utils::get_current_thread_id()) + "] ";
		std::string ctag = tag;
		ctag.resize(16, ' ');
		std::cout << static_cast<int>(level) << ':' << ctag << '\t' << tid << ' ' << message << std::endl;
	}

	virtual void store(const infrastructure::Logger::Level level, const std::string& tag, const std::vector<std::string>& messages) noexcept final override
	{
		for (const auto& message : messages)
			store(level, tag, message);
	}
};

}
}
}

