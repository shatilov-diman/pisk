// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module script of the project pisk.
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
#include <pisk/utils/algorithm_utils.h>
#include <pisk/utils/json_utils.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/script/Script.h>


namespace pisk
{
namespace script
{
	template <typename BaseStrategy>
	class PushChangesEngineStrategy :
		public BaseStrategy
	{
	public:
		template <typename ... TArgs>
		PushChangesEngineStrategy(TArgs& ... args):
			BaseStrategy(args...)
		{}

	protected:
		using Configure = typename BaseStrategy::Configure;

		virtual Configure on_init_app() override
		{
			if (not this->get_script_manager().register_external_function("script", "pisk", "push_changes", std::bind(&PushChangesEngineStrategy::push_changes_wrapper, this, std::placeholders::_1)))
				infrastructure::Logger::get().warning("script", "Unalbe to register 'push_changes' external function");

			return BaseStrategy::on_init_app();
		}

		virtual void on_deinit_app() override
		{
			BaseStrategy::on_deinit_app();
			if (not this->get_script_manager().unregister_external_function("script", "pisk", "push_changes"))
				infrastructure::Logger::get().warning("script", "Unalbe to unregister 'push_changes' external function");
		}

		Results push_changes_wrapper(const Arguments& arguments)
		{
			if (arguments.size() != 1)
			{
				infrastructure::Logger::get().warning("script", "Unexpected argumens count while call 'push_changes'");
				throw infrastructure::InvalidArgumentException();
			}
			if (not arguments[0].is_dictionary())
			{
				infrastructure::Logger::get().warning("script", "'push_changes' required only table with string as key");
				throw infrastructure::InvalidArgumentException();
			}

			this->push_changes(arguments[0]);

			return {};
		}
	};


	template <typename BaseStrategy>
	class LogEngineStrategy :
		public BaseStrategy
	{
	public:
		template <typename ... TArgs>
		LogEngineStrategy(TArgs& ... args):
			BaseStrategy(args...)
		{}

	protected:
		using Configure = typename BaseStrategy::Configure;

		virtual Configure on_init_app() override
		{
			if (not this->get_script_manager().register_external_function("script", "pisk", "log", std::bind(&LogEngineStrategy::log, this, std::placeholders::_1)))
				infrastructure::Logger::get().warning("script", "Unalbe to register 'push_changes' external function");

			return BaseStrategy::on_init_app();
		}

		virtual void on_deinit_app() override
		{
			BaseStrategy::on_deinit_app();
			if (not this->get_script_manager().unregister_external_function("script", "pisk", "log"))
				infrastructure::Logger::get().warning("script", "Unalbe to unregister 'push_changes' external function");
		}

		Results log(const Arguments& arguments)
		{
			if (arguments.size() != 2)
			{
				infrastructure::Logger::get().error("script", "Unexpected count of arguments for 'log' external function");
				return {};
			}
			if (not arguments[0].is_string())
			{
				infrastructure::Logger::get().error("script", "Unexpected arguments for 'log' external function");
				return {};
			}

			const infrastructure::Logger::Level loglevel = arg_to_loglevel(arguments[0].as_keystring());
			infrastructure::Logger::get().log(loglevel, "script log", utils::json::to_string(arguments[1]).c_str());
			return {};
		}
		static infrastructure::Logger::Level arg_to_loglevel(const utils::keystring& loglevel)
		{
			if (loglevel == "spam")
				return infrastructure::Logger::Level::Spam;
			if (loglevel == "debug")
				return infrastructure::Logger::Level::Debug;
			if (loglevel == "info")
				return infrastructure::Logger::Level::Information;
			if (loglevel == "warning")
				return infrastructure::Logger::Level::Warning;
			if (loglevel == "error")
				return infrastructure::Logger::Level::Error;
			return infrastructure::Logger::Level::Information;
		}
	};
}// namespace script
}// namespace pisk
