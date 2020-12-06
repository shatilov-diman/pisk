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
				const utils::keystring helpmember {"help"};

				if (not this->get_script_manager().register_external_function(
					"script", nameprefix, helpmember,
					[this, service](const auto& args) { return this->help(service, args); }
				))
				{
					logger::warning("script", "Unalbe to register '{}:{}' external function", nameprefix, helpmember);
				}

				for (const utils::keystring& member : service->get_members())
					if (not this->get_script_manager().register_external_function(
						"script", nameprefix, member,
						[service, member](const auto& args) { return service->execute(member, args); }
					))
					{
						logger::warning("script", "Unalbe to register '{}:{}' external function", nameprefix, member);
					}

				if (not service->get_signalers().empty())
				{
					const utils::keystring member {"subscribe"};
					//for (const utils::keystring& signaler : service->get_signalers())
					if (not this->get_script_manager().register_external_function(
						"script", nameprefix, member,
						[this, service](const auto& args) { return this->subscribe_wrapper(service, args); }
					))
					{
						logger::warning("script", "Unalbe to register '{}:{}' external function", nameprefix, member);
					}
				}
			}
			return BaseStrategy::on_init_app();
		}

		virtual void on_deinit_app() final override
		{
			BaseStrategy::on_deinit_app();
			for (const auto& pr : services2go)
			{
				Service2GoPtr service = pr.second;
				const utils::keystring nameprefix { "pisk_" + pr.first.get_content() };
				const utils::keystring helpmember {"help"};

				if (not this->get_script_manager().unregister_external_function("script", nameprefix, helpmember))
					logger::warning("script", "Unalbe to unregister '{}:{}' external function", nameprefix, helpmember);

				for (const utils::keystring& member : service->get_members())
					if (not this->get_script_manager().unregister_external_function("script", nameprefix, member))
					{
						logger::warning("script", "Unalbe to unregister '{}:{}' external function", nameprefix, member);
					}

				if (not service->get_signalers().empty())
				{
					const utils::keystring member { "subscribe" };
					if (not this->get_script_manager().unregister_external_function("script", nameprefix, member))
					{
						logger::warning("script", "Unalbe to unregister '{}:{}' external function", nameprefix, member);
					}
				}
			}
		}

		Results help(Service2GoPtr service, const Arguments& arguments)
		{
			if (arguments.size() > 1)
			{
				logger::error("script", "Unexpected count of arguments for 'log' external function");
				return {};
			}
			if (arguments.size() == 0)
				return {service->help()};
			else
			{
				if (not arguments[0].is_string())
				{
					logger::error("script", "Unexpected arguments for 'help' external function");
					return {};
				}
				return {service->help(arguments[0].as_keystring())};
			}
		}

		Results subscribe_wrapper(Service2GoPtr service, const Arguments& arguments)
		{
			if (arguments.size() != 2)
			{
				logger::error("script", "Unexpected count of arguments for 'subscribe' external function");
				return {};
			}
			if (not arguments[0].is_string() and not arguments[1].is_string())
			{
				logger::error("script", "Unexpected arguments for 'subscribe' external function");
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
