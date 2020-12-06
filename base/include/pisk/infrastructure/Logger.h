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

		class MessageMaker : public utils::noncopyable
		{
			std::vector<std::string> lines;
		public:
			template <typename ...TArgs>
			MessageMaker& add_line(const std::string& format, TArgs&& ... args)
			{
				lines.push_back(utils::string::format(format, std::forward<TArgs>(args)...));
				return *this;
			}
			void commit(const Level level, const std::string& tag)
			{
				Logger::log(level, tag, lines);
			}
		};

		static bool is_level_filtered(const Level new_level) threadsafe noexcept;

		static void set_log_level(const Level new_level) threadsafe noexcept;

		static void set_log_storage(std::unique_ptr<LogStorage> storage) threadsafe noexcept;

		static void log(const Level level, const std::string& tag, const std::string& message) threadsafe noexcept;

		static void log(const Level level, const std::string& tag, const std::vector<std::string>& messages) threadsafe noexcept;

		static MessageMaker log()
		{
			return {};
		}

		template <typename ...TArgs>
		static void log_format(const Level level, const std::string& tag, const std::string& format, TArgs&& ... args) threadsafe noexcept {
			if (is_level_filtered(level))
				return;
			const std::string message = utils::string::format(format, std::forward<TArgs>(args)...);
			log(level, tag, message);
		}
		template <typename ...TArgs>
		static void critical(const std::string& tag, const std::string& format, TArgs&& ... args) threadsafe noexcept {
			log_format(Level::Critical, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		static void error(const std::string& tag, const std::string& format, TArgs&& ... args) threadsafe noexcept {
			log_format(Level::Error, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		static void warning(const std::string& tag, const std::string& format, TArgs&& ... args) threadsafe noexcept {
			log_format(Level::Warning, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		static void info(const std::string& tag, const std::string& format, TArgs&& ... args) threadsafe noexcept {
			log_format(Level::Information, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		static void debug(const std::string& tag, const std::string& format, TArgs&& ... args) threadsafe noexcept {
			log_format(Level::Debug, tag, format, std::forward<TArgs>(args)...);
		}
		template <typename ...TArgs>
		static void spam(const std::string& tag, const std::string& format, TArgs&& ... args) threadsafe noexcept {
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
	using logger = infrastructure::Logger;
}

