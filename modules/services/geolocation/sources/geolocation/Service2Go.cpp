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


#include <pisk/defines.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentsLoader.h>

#include <pisk/script/Service2Go.h>
#include <pisk/geolocation/Service.h>

using namespace pisk::tools;

namespace pisk
{
namespace services
{
namespace geolocation
{
	class Service2Go :
		public core::Component,
		public script::Service2Go
	{
		ServicePtr service;

	public:
		explicit Service2Go(const ServicePtr& _service):
			service(_service)
		{
			if (service == nullptr)
				throw infrastructure::NullPointerException();
		}

	private:
		virtual void release()
		{
			delete this;
		}

		virtual utils::keystring help()
		{
			return "Geolocation service. Uses GoogleAPI:geolocation by IP for desktops and internal services for mobiles.";
		}

		virtual utils::keystring help(const utils::keystring& keyword)
		{
			auto found1 = signalers.find(keyword);
			if (found1 == signalers.end())
			{
				auto found2 = members.find(keyword);
				if (found2 == members.end())
				{
					logger::warning("geolocation2go", "Unknown keyword '{}'", keyword);
					throw infrastructure::InvalidArgumentException();
				}
				return found2->second.help_message;
			}
			return found1->second.help_message;
		}

	private:
		struct SignalerTraits
		{
			const utils::keystring help_message;
			const std::function<pisk::utils::auto_unsubscriber (const utils::keystring& signaler, std::function<void (const script::Arguments& argumens)> callback)> subscriber;
		};

		const std::map<utils::keystring, SignalerTraits> signalers {
			{"update_location", SignalerTraits {
				"Called on each update location. void (location:{timestamp:int,accuracy:double,latitude:double,longitude:double,provider:string})",
				[this](const utils::keystring& signaler, std::function<void (const script::Arguments& argumens)> callback) -> pisk::utils::auto_unsubscriber {
					return service->on_update_location.subscribe(std::bind(
						&Service2Go::on_update<Location>,
						signaler,
						callback,
						std::placeholders::_1
					));
				}
			} },
			{"update_status", SignalerTraits {
				"Called on each status update. void (timestamp:int,status:string)",
				[this](const utils::keystring& signaler, std::function<void (const script::Arguments& argumens)> callback) -> pisk::utils::auto_unsubscriber {
					return service->on_status_update.subscribe(std::bind(
						&Service2Go::on_update<Status>,
						signaler,
						callback,
						std::placeholders::_1
					));
				}
			} },
			{"update_error", SignalerTraits {
				"Called on each error occured. void (error:{timestamp:int,msg:string,domain:string,code:int,provider:string})",
				[this](const utils::keystring& signaler, std::function<void (const script::Arguments& argumens)> callback) -> pisk::utils::auto_unsubscriber {
					return service->on_update_error.subscribe(std::bind(
						&Service2Go::on_update<Error>,
						signaler,
						callback,
						std::placeholders::_1
					));
				}
			} }
		};

		template <typename Input>
		static void on_update(const utils::keystring& signaler, std::function<void (const script::Arguments& argumens)> callback, const Input& input)
		{
			const utils::property& signalername = signaler;
			const utils::property& inputser = to_property(input);
			callback({inputser, signalername});
		}

		virtual std::vector<utils::keystring> get_signalers()
		{
			std::vector<utils::keystring> out;
			for (const auto& pr : signalers)
				out.push_back(pr.first);
			return out;
		}

		virtual pisk::utils::auto_unsubscriber subscribe(const utils::keystring& signaler, std::function<void (const script::Arguments& argumens)> callback)
		{
			auto found = signalers.find(signaler);
			if (found == signalers.end())
			{
				logger::warning("geolocation2go", "Unknown signaler '{}'", signaler);
				throw infrastructure::InvalidArgumentException();
			}
			return found->second.subscriber(signaler, callback);
		}

	private:
		struct MemberTraits
		{
			const utils::keystring help_message;
			const std::function<script::Results (const script::Arguments& argumens)> executor;
		};

		const std::map<utils::keystring, MemberTraits> members {
			{"get_available_providers", {
				"Returns available providers lists",
				std::bind(&Service2Go::get_available_providers, this, std::placeholders::_1)
			} },
			{"enable_provider", {
				"Subscribe to updates from provider",
				std::bind(&Service2Go::enable_provider, this, std::placeholders::_1)
			} },
			{"disable_provider", {
				"Unsubscribe to updates from provider",
				std::bind(&Service2Go::disable_provider, this, std::placeholders::_1)
			} },
			{"get_location", {
				"Get last location",
				std::bind(&Service2Go::get_location, this, std::placeholders::_1)
			} },
			{"get_error", {
				"Get last error per provider",
				std::bind(&Service2Go::get_error, this, std::placeholders::_1)
			} },
		};

		virtual std::vector<utils::keystring> get_members()
		{
			std::vector<utils::keystring> out;
			for (const auto& pr : members)
				out.push_back(pr.first);
			return out;
		}

		virtual script::Results execute(const utils::keystring& member, const script::Arguments& arguments)
		{
			auto found = members.find(member);
			if (found == members.end())
			{
				logger::warning("geolocation2go", "Unknown member '{}'", member);
				throw infrastructure::InvalidArgumentException();
			}
			return found->second.executor(arguments);
		}
		script::Results get_available_providers(const script::Arguments& arguments)
		{
			if (arguments.size() != 0)
			{
				logger::error("script2go", "Unexpected count of arguments for 'get_available_providers' external function");
				return {};
			}
			utils::property out;
			const auto& providers = service->get_available_providers();
			for (auto provider : providers)
			{
				out[out.size()] = to_string(provider);
			}
			return { out };
		}
		script::Results enable_provider(const script::Arguments& arguments)
		{
			return switch_provider(arguments, true);
		}
		script::Results disable_provider(const script::Arguments& arguments)
		{
			return switch_provider(arguments, false);
		}
		script::Results switch_provider(const script::Arguments& arguments, const bool enable)
		{
			if (arguments.size() != 1)
			{
				logger::error("script2go", "Unexpected count of arguments for 'enable_provider/disable_provider' external function");
				throw infrastructure::InvalidArgumentException();
			}
			if (not arguments[0].is_string())
			{
				logger::error("script2go", "Unexpected argument type at 'enable_provider/disable_provider' external function");
				throw infrastructure::InvalidArgumentException();
			}
			const auto& provider = to_provider(arguments[0].as_keystring());
			const bool result = enable
						? service->enable_provider(provider)
						: service->disable_provider(provider);
			return { result };
		}
		script::Results get_location(const script::Arguments& arguments)
		{
			if (arguments.size() != 0)
			{
				logger::error("script2go", "Unexpected count of arguments for 'get_location' external function");
				throw infrastructure::InvalidArgumentException();
			}
			const auto& location = service->get_location();
			return { to_property(location) };
		}

		script::Results get_error(const script::Arguments& arguments)
		{
			if (arguments.size() != 1)
			{
				logger::error("script2go", "Unexpected count of arguments for 'get_available_providers' external function");
				throw infrastructure::InvalidArgumentException();
			}
			if (not arguments[0].is_string())
			{
				logger::error("script2go", "Unexpected argument type at 'get_error' external function");
				throw infrastructure::InvalidArgumentException();
			}
			const auto& provider = to_provider(arguments[0].as_keystring());
			const auto& error = service->get_error(provider);
			return { to_property(error) };
		}

		static utils::property to_property(const Location& location)
		{
			utils::property out;
			out["timestamp"] = std::to_string(location.timestamp.time_since_epoch().count());
			out["accuracy"] = location.accuracy;
			out["latitude"] = location.latitude;
			out["longitude"] = location.longitude;
			out["provider"] = to_string(location.provider);
			return out;
		}
		static utils::property to_property(const Status& status)
		{
			utils::property out;
			out["timestamp"] = std::to_string(status.timestamp.time_since_epoch().count());
			out["status"] = to_string(status.status);
			out["provider"] = to_string(status.provider);
			return out;
		}
		static utils::property to_property(const Error& error)
		{
			utils::property out;
			out["timestamp"] = std::to_string(error.timestamp.time_since_epoch().count());
			out["msg"] = error.msg;
			out["provider"] = to_string(error.provider);
			out["domain"] = to_string(error.domain);
			out["code"] = error.code;
			return out;
		}

		static utils::keystring to_string(const Provider provider)
		{
			switch (provider)
			{
				case Provider::None:
					return "None";
				case Provider::IP:
					return "IP";
				case Provider::PhoneCell:
					return "PhoneCell";
				case Provider::Sattelite:
					return "Sattelite";
				case Provider::Manual:
					return "Manual";
				default:
					throw infrastructure::InvalidArgumentException();
			}
		}

		static Provider to_provider(const utils::keystring provider)
		{
			if (provider == "None")
				return Provider::None;
			if (provider == "IP")
				return Provider::IP;
			if (provider == "PhoneCell")
				return Provider::PhoneCell;
			if (provider == "Sattelite")
				return Provider::Sattelite;
			if (provider == "Manual")
				return Provider::Manual;

			throw infrastructure::InvalidArgumentException();
		}

		static utils::keystring to_string(const ProviderStatus status)
		{
			switch (status)
			{
				case ProviderStatus::None:
					return "None";
				case ProviderStatus::Enabled:
					return "Enabled";
				case ProviderStatus::NotAllowed:
					return "NotAllowed";
				case ProviderStatus::Disabled:
					return "Disabled";
				default:
					throw infrastructure::InvalidArgumentException();
			}
		}

		static utils::keystring to_string(const Error::Domain provider)
		{
			switch (provider)
			{
				case Error::Domain::None:
					return "None";
				case Error::Domain::Internal:
					return "Internal";
				case Error::Domain::Web:
					return "Web";
				case Error::Domain::Http:
					return "Http";
				case Error::Domain::Service:
					return "Service";
				default:
					throw infrastructure::InvalidArgumentException();
			}
		}
	};
}//namespace geolocation
}//namespace services
}//namespace pisk


SafeComponentPtr __cdecl geolocation_service2go_factory(const pisk::tools::ServiceRegistry& temp_sl, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&geolocation_service2go_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	using namespace pisk::services::geolocation;

	const ServicePtr& service = temp_sl.get<Service>();
	if (service == nullptr)
	{
		pisk::logger::warning("geolocation2go", "Unable to locate 'geolocation' service");
		return {};
	}

	return factory.make<pisk::services::geolocation::Service2Go>(service);
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_geolocation_service2go_factory()
{
	static_assert(std::is_convertible<decltype(&get_geolocation_service2go_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &geolocation_service2go_factory;
}

