// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module pisk of the project pisk.
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


#include <pisk/bdd.h>

#include <pisk/tools/Application.h>
#include <pisk/system/ResourceManager.h>
#include <pisk/tools/MainLoop.h>

#include <pisk/geolocation/Service.h>

#include <chrono>
#include <future>
#include <thread>

#include "ServiceTest.h"

using namespace igloo;
namespace components = pisk::tools::components;
namespace geolocation = pisk::services::geolocation;

static const pisk::utils::property geo_config = pisk::utils::property::dictionary {
	{"IP", pisk::utils::property::dictionary {
		//Please, do not use this key. In case, when it will be used too much, I will disable it.
		{"URL", "https://www.googleapis.com/geolocation/v1/geolocate?key=AIzaSyB3PZ5S3FkF_PinEmV9ZUpfmBNkW6xRmfI"},
	}}
};

static const components::DescriptionsList desc_list {
	components::Description {"service", "http", "http", "get_http_service_factory"},
	components::Description {"service", "geolocation", "geolocation", "get_geolocation_service_factory", geo_config},
};

Context(check_geolocation_service) {
	Spec(check_ip) {

		geolocation::Location location;

		std::string fail_msg =
		RunAppForTestService<geolocation::Service>(desc_list, [&location](const geolocation::ServicePtr& service) -> std::string {
			service->enable_provider(geolocation::Provider::IP);
			while (true)
			{
				const auto& loc = service->get_location();
				if (loc.provider != geolocation::Provider::None)
				{
					location = loc;
					break;
				}
				const auto& error = service->get_error(geolocation::Provider::IP);
				if (error.domain != geolocation::Error::Domain::None)
					return error.msg.get_content();
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			return "";
		});
		Assert::That(fail_msg, Is().EqualTo(""));
		Assert::That(location.provider, Is().EqualTo(geolocation::Provider::IP));
		Assert::That(location.latitude, Is().Not().EqualTo(0.));
		Assert::That(location.longitude, Is().Not().EqualTo(0.));
	}
	Spec(check_empty_url) {

		static const pisk::utils::property wrong_geo_config = pisk::utils::property::dictionary {
			{"IP", pisk::utils::property::dictionary {
				{"URL", ""},
			}}
		};
		static const components::DescriptionsList wrong_desc_list {
		components::Description {"service", "http", "http", "get_http_service_factory"},
			components::Description {"service", "geolocation", "geolocation", "get_geolocation_service_factory", wrong_geo_config},
		};

		geolocation::Location location;

		std::string fail_msg =
		RunAppForTestService<geolocation::Service>(wrong_desc_list, [&location](const geolocation::ServicePtr&) -> std::string {
			return "We should not be here";
		});
		Assert::That(fail_msg, Is().EqualTo("Service not found"));
	}
	Spec(check_no_url) {

		static const pisk::utils::property wrong_geo_config = pisk::utils::property::dictionary {
			{"IP", pisk::utils::property::dictionary { }}
		};
		static const components::DescriptionsList wrong_desc_list {
			components::Description {"service", "http", "http", "get_http_service_factory"},
			components::Description {"service", "geolocation", "geolocation", "get_geolocation_service_factory", wrong_geo_config},
		};

		geolocation::Location location;

		std::string fail_msg =
		RunAppForTestService<geolocation::Service>(wrong_desc_list, [&location](const geolocation::ServicePtr&) -> std::string {
			return "We should not be here";
		});
		Assert::That(fail_msg, Is().EqualTo("Service not found"));
	}
	Spec(check_no_ip) {

		static const pisk::utils::property wrong_geo_config = pisk::utils::property::dictionary { };
		static const components::DescriptionsList wrong_desc_list {
			components::Description {"service", "http", "http", "get_http_service_factory"},
			components::Description {"service", "geolocation", "geolocation", "get_geolocation_service_factory", wrong_geo_config},
		};

		geolocation::Location location;

		std::string fail_msg =
		RunAppForTestService<geolocation::Service>(wrong_desc_list, [&location](const geolocation::ServicePtr&) -> std::string {
			return "We should not be here";
		});
		Assert::That(fail_msg, Is().EqualTo("Service not found"));
	}
};

