// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
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


#include <pisk/infrastructure/Logger.h>

#include <atomic>
#include <mutex>

namespace pisk
{
namespace infrastructure
{
namespace impl
{
	class Logger :
		public infrastructure::Logger
	{
		std::mutex guard;
		std::unique_ptr<LogStorage> storage;
		std::atomic<Level> filtered_level = {Level::Information};

		virtual bool is_level_filtered(const Level check_level) const threadsafe noexcept final override
		{
			return check_level > filtered_level;
		}

		virtual void set_log_level(const Level new_level) threadsafe noexcept final override
		{
			filtered_level = new_level;
		}

		virtual void set_log_storage(std::unique_ptr<LogStorage> _storage) threadsafe noexcept final override
		{
			std::unique_lock<std::mutex> lock(guard);
			storage = std::move(_storage);
		}

		virtual void log(const Level level, const std::string& tag, const std::string& message) threadsafe noexcept final override
		{
			std::unique_lock<std::mutex> lock(guard);
			if (storage != nullptr)
				storage->store(level, tag, message);
		}
		virtual void log(const Level level, const std::string& tag, const std::vector<std::string>& messages) threadsafe noexcept final override
		{
			std::unique_lock<std::mutex> lock(guard);
			if (storage != nullptr)
				storage->store(level, tag, messages);
		}
	};
}//namespace impl

	Logger& Logger::get()
	{
		static impl::Logger instance;
		return instance;
	}
}
}
