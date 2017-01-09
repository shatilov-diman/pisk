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


#pragma once

#include "../defines.h"
#include "../utils/noncopyable.h"
#include "../utils/algorithm_utils.h"
#include "Exception.h"

#include <memory>
#include <vector>
#include <string>

namespace pisk
{
namespace infrastructure
{
	class LogStorage;

	class EXPORT Logger
	{
	public:
		enum class Level
		{
			Silent,
			Critical,
			Error,
			Warning,
			Information,
			Debug,
			Spam,
		};

		static Logger& get();

		class MessageMaker : public utils::noncopyable
		{
			Logger& logger;
			std::vector<std::string> lines;
		public:
			explicit MessageMaker(Logger& logger) :
				logger(logger)
			{}

			template <typename ...TArgs>
			MessageMaker& add_line(const std::string& format, TArgs... args)
			{
				lines.push_back(utils::string::format(format, std::forward<TArgs>(args)...));
				return *this;
			}
			void commit(const Level level, const std::string& tag)
			{
				using namespace pisk::utils::algorithm;
				logger.log(level, tag, lines);
			}
		};

		virtual bool is_level_filtered(const Level new_level) const threadsafe noexcept = 0;

		virtual void set_log_level(const Level new_level) threadsafe noexcept = 0;

		virtual void set_log_storage(std::unique_ptr<LogStorage> storage) threadsafe noexcept = 0;

		virtual void log(const Level level, const std::string& tag, const std::string& message) threadsafe noexcept = 0;

		virtual void log(const Level level, const std::string& tag, const std::vector<std::string>& messages) threadsafe noexcept = 0;

		MessageMaker log()
		{
			return MessageMaker(*this);
		}

		template <typename ...TArgs>
		void log_format(const Level level, const std::string& tag, const std::string& format, TArgs... args) threadsafe noexcept {
			if (is_level_filtered(level))
				return;
			const std::string message = utils::string::format(format, std::forward<TArgs>(args)...);
			log(level, tag, message);
		}
		template <typename ...TArgs>
		void critical(const std::string& tag, const std::string& format, TArgs... args) threadsafe noexcept {
			log_format(Level::Critical, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		void error(const std::string& tag, const std::string& format, TArgs... args) threadsafe noexcept {
			log_format(Level::Error, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		void warning(const std::string& tag, const std::string& format, TArgs... args) threadsafe noexcept {
			log_format(Level::Warning, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		void info(const std::string& tag, const std::string& format, TArgs... args) threadsafe noexcept {
			log_format(Level::Information, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		void debug(const std::string& tag, const std::string& format, TArgs... args) threadsafe noexcept {
			log_format(Level::Debug, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		void spam(const std::string& tag, const std::string& format, TArgs... args) threadsafe noexcept {
			log_format(Level::Spam, tag, format, std::forward<TArgs>(args)...);
		}
	};

	class LogStorage
	{
	public:
		virtual void store(const Logger::Level level, const std::string& tag, const std::string& message) noexcept = 0;

		virtual void store(const Logger::Level level, const std::string& tag, const std::vector<std::string>& messages) noexcept = 0;
	};
}
}
