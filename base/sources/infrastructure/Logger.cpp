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


#include <pisk/infrastructure/Logger.h>

#include <atomic>
#include <mutex>

namespace pisk
{
namespace infrastructure
{
	static std::mutex guard;
	static std::unique_ptr<LogStorage> storage;
	static std::atomic<Logger::Level> filtered_level = {Logger::Level::Information};

	bool Logger::is_level_filtered(const Level check_level) threadsafe noexcept
	{
		return check_level > filtered_level;
	}

	void Logger::set_log_level(const Level new_level) threadsafe noexcept
	{
		filtered_level = new_level;
	}

	void Logger::set_log_storage(std::unique_ptr<LogStorage> _storage) threadsafe noexcept
	{
		std::unique_lock<std::mutex> lock(guard);
		storage = std::move(_storage);
	}

	void Logger::log(const Level level, const std::string& tag, const std::string& message) threadsafe noexcept
	{
		std::unique_lock<std::mutex> lock(guard);
		if (storage != nullptr)
			storage->store(level, tag, message);
	}
	void Logger::log(const Level level, const std::string& tag, const std::vector<std::string>& messages) threadsafe noexcept
	{
		std::unique_lock<std::mutex> lock(guard);
		if (storage != nullptr)
			storage->store(level, tag, messages);
	}
}
}

