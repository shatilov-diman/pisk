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

#include <pisk/script/Service2Go.h>


namespace pisk
{
namespace script
{
	using Service2GoContainer = std::map<utils::keystring, Service2GoPtr>;

	template <typename BaseStrategy>
	class S2GEngineStrategy :
		public BaseStrategy
	{
		Service2GoContainer services2go;

		std::vector<pisk::utils::auto_unsubscriber> subscriptions;

	public:
		template <typename ... TArgs>
		S2GEngineStrategy(const Service2GoContainer& services2go, TArgs& ... args):
			BaseStrategy(args...),
			services2go(services2go)
		{}

	private:
		using Configure = typename BaseStrategy::Configure;

		virtual Configure on_init_app() final override
		{
			for (const auto& pr : services2go)
			{
				Service2GoPtr service = pr.second;
				const utils::keystring nameprefix { "pisk_" + pr.first.get_content() };

				if (not this->get_script_manager().register_external_function(
					"script",
					nameprefix,
					"help",
					std::bind(
						&S2GEngineStrategy::help,
						this,
						service,
						std::placeholders::_1
					)
				))
				{
					infrastructure::Logger::get().warning("script", "Unalbe to register 'help' external function");
				}

				for (const utils::keystring& member : service->get_members())
					if (not this->get_script_manager().register_external_function(
						"script",
						nameprefix,
						member,
						std::bind(
							&Service2Go::execute,
							service,
							member,
							std::placeholders::_1
						)
					))
					{
						infrastructure::Logger::get().warning("script", "Unalbe to register 'push_changes' external function");
					}

				if (not service->get_signalers().empty())
				{
					//for (const utils::keystring& signaler : service->get_signalers())
					if (not this->get_script_manager().register_external_function(
						"script",
						nameprefix,
						"subscribe",
						std::bind(
							&S2GEngineStrategy::subscribe_wrapper,
							this,
							service,
							std::placeholders::_1
						)
					))
					{
						infrastructure::Logger::get().warning("script", "Unalbe to register 'push_changes' external function");
					}
				}
			}
			return BaseStrategy::on_init_app();
		}

		virtual void on_deinit_app() final override
		{
			BaseStrategy::on_deinit_app();
			if (not this->get_script_manager().unregister_external_function("script", "pisk", "help"))
				infrastructure::Logger::get().warning("script", "Unalbe to unregister 'push_changes' external function");

			for (const auto& pr : services2go)
			{
				Service2GoPtr service = pr.second;
				const utils::keystring nameprefix { "pisk_" + pr.first.get_content() };

				for (const utils::keystring& member : service->get_members())
					if (not this->get_script_manager().unregister_external_function(
						"script",
						nameprefix,
						member
					))
					{
						infrastructure::Logger::get().warning("script", "Unalbe to register 'push_changes' external function");
					}
				if (not service->get_signalers().empty())
				{
					if (not this->get_script_manager().unregister_external_function(
						"script",
						nameprefix,
						"subscribe"
					))
					{
						infrastructure::Logger::get().warning("script", "Unalbe to register 'push_changes' external function");
					}
				}
			}
		}

		Results help(Service2GoPtr service, const Arguments& arguments)
		{
			if (arguments.size() > 1)
			{
				infrastructure::Logger::get().error("script", "Unexpected count of arguments for 'log' external function");
				return {};
			}
			if (arguments.size() == 0)
				return {service->help()};
			else
			{
				if (not arguments[0].is_string())
				{
					infrastructure::Logger::get().error("script", "Unexpected arguments for 'help' external function");
					return {};
				}
				return {service->help(arguments[0].as_keystring())};
			}
		}

		Results subscribe_wrapper(Service2GoPtr service, const Arguments& arguments)
		{
			if (arguments.size() != 2)
			{
				infrastructure::Logger::get().error("script", "Unexpected count of arguments for 'subscribe' external function");
				return {};
			}
			if (not arguments[0].is_string() and not arguments[1].is_string())
			{
				infrastructure::Logger::get().error("script", "Unexpected arguments for 'subscribe' external function");
				return {};
			}
			utils::keystring callbackname = arguments[1].as_keystring();
			pisk::utils::auto_unsubscriber subscription = service->subscribe(
				arguments[0].as_keystring(),
				[this, callbackname](Arguments arguments) -> void {
					this->execute("script", callbackname.c_str(), arguments);
				}
			);
			subscriptions.push_back(subscription);
			return {};
		}
	};
}// namespace script
}// namespace pisk
