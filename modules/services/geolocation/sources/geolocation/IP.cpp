// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module geolocation of the project pisk.
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


#include <pisk/defines.h>

#include <pisk/utils/keystring.h>
#include <pisk/utils/json_utils.h>
#include <pisk/utils/property_tree.h>

#include <pisk/infrastructure/Logger.h>

#include <pisk/http/Service.h>

#include <chrono>
#include <thread>

#include "ServiceImpl.h"

namespace pisk
{
namespace services
{
namespace geolocation
{
namespace googleapi
{
	class ProviderByIP :
		public GeolocationProvider
	{
		pisk::services::http::ServicePtr http_service;
		const http::URL request_url;

		std::unique_ptr<std::thread> worker;
		bool stop_flag = false;
	public:
		ProviderByIP(const pisk::services::http::ServicePtr& http_service, const pisk::utils::property& config) :
			http_service(http_service),
			request_url(get_request_url(config))
		{}

		virtual ~ProviderByIP()
		{
			stop_locate();
		}

		virtual bool start_locate() final override
		{
			if (worker != nullptr)
				return false;
			reset_stop_flag();
			worker = std::make_unique<std::thread>(std::bind(&ProviderByIP::run, this));
			return true;
		}

		virtual bool stop_locate() final override
		{
			if (worker == nullptr)
				return false;
			set_stop_flag();
			worker->join();
			worker.reset();
			return true;
		}

	private:
		static http::URL get_request_url(const pisk::utils::property& config)
		{
			if (not config["IP"]["URL"].is_string())
			{
				pisk::infrastructure::Logger::get().warning("script", "'IP/URL' key not found in the config ");
				throw infrastructure::InvalidArgumentException();
			}
			const auto& url = config["IP"]["URL"].as_keystring();
			if (url.empty())
			{
				pisk::infrastructure::Logger::get().error("script", "Value 'IP/URL' is empty");
				throw infrastructure::InvalidArgumentException();
			}
			return http::to_url(url);
		}
		void run()
		{
			do
			{
				auto request = request_update_location();
				while (request.wait_for(std::chrono::milliseconds{100}) != std::future_status::ready)
					if (is_stopped())
						return;
				const http::Response& response = request.get();
				on_response(response);
			}
			while(wait_for_stop(60) == false);
		}
		bool wait_for_stop(const std::size_t seconds)
		{
			for (std::size_t index = 0; index < 10 * seconds; ++index)
			{
				if (is_stopped())
					return true;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			return false;
		}
		std::future<http::Response> request_update_location()
		{
			const http::Headers headers {
				"Content-Type: application/json",
			};
			const std::string geolocation_request = utils::json::to_string(get_detect_info());
			const infrastructure::DataBuffer body {geolocation_request.begin(), geolocation_request.end()};

			return http_service->request(http::Request {request_url, headers, body, http::Method::POST, nullptr});
		}
		void on_response(const http::Response& response)
		{
			const utils::property resp = utils::json::parse_json_to_property(response.body);
			if (response.error_code != http::ErrorCode::OK)
			{
				Error error;
				error.timestamp = std::chrono::system_clock::now();
				error.provider = Provider::IP;
				error.domain = Error::Domain::Web;
				error.code = static_cast<int>(response.error_code);
				this->on_update_error.emit(error);

				Status status;
				status.timestamp = error.timestamp;
				status.provider = error.provider;
				status.status = ProviderStatus::Disabled;
				this->on_status_update.emit(status);
			}
			else if (response.status_code != http::Status::OK)
			{
				Error error;
				error.timestamp = std::chrono::system_clock::now();
				error.provider = Provider::IP;
				error.domain = Error::Domain::Http;
				error.code = static_cast<int>(response.status_code);
				if (resp["error"]["message"].is_string())
					error.msg = resp["error"]["message"].as_keystring();
				else
					error.msg = response.headers[0];
				this->on_update_error.emit(error);

				Status status;
				status.timestamp = error.timestamp;
				status.provider = error.provider;
				status.status = ProviderStatus::NotAllowed;
				this->on_status_update.emit(status);
			}
			else
			{
				Location location;
				location.timestamp = std::chrono::system_clock::now();
				location.provider = Provider::IP;
				location.accuracy = resp["accuracy"].as_double();
				location.longitude = resp["location"]["lng"].as_double();
				location.latitude = resp["location"]["lat"].as_double();
				this->on_update_location.emit(location);

				Status status;
				status.timestamp = location.timestamp;
				status.provider = location.provider;
				status.status = ProviderStatus::Enabled;
				this->on_status_update.emit(status);
			}
		}

	private:
		utils::property get_detect_info() const
		{
			utils::property out;
			out["considerIp"] = true;
			utils::property::merge(out, get_phone_info());
			utils::property::merge(out, get_wifi_info());
			return out;
		}
		utils::property get_phone_info() const
		{
			utils::property out;
			//out["homeMobileCountryCode"] = 357;
			//out["homeMobileNetworkCode"] = 0;
			//out["radioType"] = "gsm";
			//out["carrier"] = "MobileTel";
			out["cellTowers"] = utils::property::array();
			return out;
		}
		utils::property get_wifi_info() const
		{
			utils::property out;
			out["wifiAccessPoints"] = utils::property::array();
			return out;
		}

	private:
		void reset_stop_flag()
		{
			stop_flag = false;
		}
		void set_stop_flag()
		{
			stop_flag = true;
		}
		bool is_stopped() const
		{
			return stop_flag;
		}
	};
}//namespace googleapi

pisk::services::geolocation::GeolocationProviderPtr create_webip_geolocation_service(const pisk::services::http::ServicePtr& http_service, const pisk::utils::property& config)
{
	return std::make_unique<pisk::services::geolocation::googleapi::ProviderByIP>(http_service, config);
}

}//namespace geolocation
}//namespace services
}//namespace pisk

