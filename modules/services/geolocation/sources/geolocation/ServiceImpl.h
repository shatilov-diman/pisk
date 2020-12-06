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
#include <pisk/utils/signaler.h>
#include <pisk/utils/property_tree.h>

#include <pisk/geolocation/Service.h>

#include <vector>
#include <mutex>
#include <map>

namespace pisk
{
namespace services
{
namespace geolocation
{
	class GeolocationProvider
	{
	public:
		LocationSignaler on_update_location;
		StatusSignaler on_status_update;
		ErrorSignaler on_update_error;

		virtual ~GeolocationProvider() {}

		virtual bool start_locate() = 0;

		virtual bool stop_locate() = 0;
	};
	using GeolocationProviderPtr = std::unique_ptr<GeolocationProvider>;
	using Providers = std::map<Provider, GeolocationProviderPtr>;

	class ServiceImpl :
		public Service
	{
		mutable std::mutex mutex;
		const Providers providers;

		std::map<Provider, utils::auto_unsubscriber> update_subscriptions;
		std::map<Provider, utils::auto_unsubscriber> status_subscriptions;
		std::map<Provider, utils::auto_unsubscriber> error_subscriptions;

		Location location = {};
		std::map<Provider, Status> status;
		std::map<Provider, Error> error;

		virtual void release() override
		{
			logger::info("geolocation", "destroy service");
			delete this;
		}

	public:
		explicit ServiceImpl(Providers&& providers):
			providers(std::move(providers))
		{
			logger::info("geolocation", "construct service");
		}

		virtual std::vector<Provider> get_available_providers() const final override
		{
			std::vector<Provider> keys;
			for (const auto& it : providers)
				keys.push_back(it.first);
			return keys;
		}

		virtual bool enable_provider(const Provider provider) threadsafe final override
		{
			logger::info("geolocation", "enable provider");
			auto it = providers.find(provider);
			if (it == providers.end())
				return false;

			std::lock_guard<std::mutex> guard(mutex);
			if (is_subscribed(provider))
				return false;
			subscribe(provider);
			error[provider] = Error {{}, {}, provider};
			return it->second->start_locate();
		}

		virtual bool disable_provider(const Provider provider) threadsafe final override
		{
			logger::info("geolocation", "disable provider");
			auto it = providers.find(provider);
			if (it == providers.end())
				return false;

			std::lock_guard<std::mutex> guard(mutex);
			if (not is_subscribed(provider))
				return false;
			unsubscribe(provider);
			return it->second->stop_locate();
		}

		virtual Location get_location() const threadsafe final override
		{
			std::lock_guard<std::mutex> guard(mutex);
			return location;
		}
		virtual Status get_status(const Provider provider) const threadsafe final override
		{
			std::lock_guard<std::mutex> guard(mutex);
			return status.at(provider);
		}
		virtual Error get_error(const Provider provider) const threadsafe final override
		{
			std::lock_guard<std::mutex> guard(mutex);
			return error.at(provider);
		}

	private:
		void subscribe(const Provider provider)
		{
			logger::info("geolocation", "subscribe provider");
			LocationSignaler& update_signaler = providers.at(provider)->on_update_location;
			update_subscriptions[provider] = update_signaler.subscribe(std::bind(&ServiceImpl::_on_update_location, this, std::placeholders::_1));

			StatusSignaler& status_signaler = providers.at(provider)->on_status_update;
			status_subscriptions[provider] = status_signaler.subscribe(std::bind(&ServiceImpl::_on_update_status, this, std::placeholders::_1));

			ErrorSignaler& error_signaler = providers.at(provider)->on_update_error;
			error_subscriptions[provider] = error_signaler.subscribe(std::bind(&ServiceImpl::_on_update_error, this, std::placeholders::_1));
		}
		void unsubscribe(const Provider provider)
		{
			logger::info("geolocation", "unsubscribe provider");
			update_subscriptions.erase(provider);
			error_subscriptions.erase(provider);
		}
		bool is_subscribed(const Provider provider) const
		{
			return update_subscriptions.find(provider) != update_subscriptions.end();
		}

	private:
		void _on_update_location(const Location& new_location)
		{
			logger::spam("geolocation", "on update location");
			bool updated = false;
			{
				std::lock_guard<std::mutex> guard(mutex);
				updated = is_better(new_location, location);
				if (updated)
					location = new_location;
			}
			if (updated)
				on_update_location.emit(new_location);
		}
		static bool is_better(const Location& new_location, const Location& old_location)
		{
			const int new_factor = static_cast<int>(new_location.provider);
			const int old_factor = static_cast<int>(old_location.provider);
			const auto delta_time = std::chrono::minutes(5) * (old_factor - new_factor);
			if (new_location.timestamp > old_location.timestamp + delta_time)
				return true;
			return new_location.accuracy < old_location.accuracy;
		}
		void _on_update_status(const Status& new_status)
		{
			logger::debug("geolocation", "on update status");
			bool updated = false;
			{
				std::lock_guard<std::mutex> guard(mutex);
				updated = is_better(new_status, status[new_status.provider]);
				if (updated)
					status[new_status.provider] = new_status;
			}
			if (updated)
				on_status_update.emit(new_status);
		}
		static bool is_better(const Status& new_status, const Status& old_status)
		{
			if (new_status.provider != old_status.provider and old_status.provider != Provider::None)
				throw infrastructure::LogicErrorException();
			return new_status.timestamp > old_status.timestamp;
		}
		void _on_update_error(const Error& new_error)
		{
			logger::error("geolocation", "on error: {}", new_error.msg);
			bool updated = false;
			{
				std::lock_guard<std::mutex> guard(mutex);
				updated = is_better(new_error, error[new_error.provider]);
				if (updated)
					error[new_error.provider] = new_error;
			}
			if (updated)
				on_update_error.emit(new_error);
		}
		static bool is_better(const Error& new_error, const Error& old_error)
		{
			if (new_error.provider != old_error.provider and old_error.provider != Provider::None)
				throw infrastructure::LogicErrorException();
			return new_error.timestamp > old_error.timestamp;
		}
	};
}
}
}

