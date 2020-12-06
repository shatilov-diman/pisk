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


#include <pisk/bdd.h>

#include <pisk/infrastructure/Exception.h>
#include <pisk/tools/Application.h>
#include <pisk/tools/MainLoop.h>

#include <pisk/system/ResourceManager.h>

#include <pisk/http/Service.h>

#include <chrono>

#include "ServiceTest.h"

static const char test_host[] = "httpbin.org";

using namespace igloo;
namespace components = pisk::tools::components;
namespace http = pisk::services::http;

static const components::DescriptionsList desc_list {
	components::Description {"service", "http", "http", "get_http_service_factory"},
};

Context(check_http_service) {
	Spec(sync_http_request) {
		std::string fail_msg =
		RunAppForTestService<http::Service>(desc_list, [](const http::ServicePtr& service) -> std::string {
			const http::URL url {"", "", test_host, "", "", "", ""};
			const http::Request request {url, {}, {}, http::Method::GET};
			const http::Response& response = service->request(request).get();
			if (response.error_code != http::ErrorCode::OK)
				return "Unexpected error_code: " + std::to_string(static_cast<int>(response.error_code));
			else if (response.status_code != http::Status::OK)
				return "Unexpected http status: " + std::to_string(static_cast<int>(response.status_code));
			return "";
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(async_http_request) {
		std::string fail_msg =
		RunAppForTestService<http::Service>(desc_list, [](const http::ServicePtr& service) -> std::string {
			const http::URL url {"", "", test_host, "", "", "", ""};
			const http::Request request {url, {}, {}, http::Method::GET};

			std::chrono::system_clock::duration sync_time;
			{
				const auto start = std::chrono::system_clock::now();
				for (std::size_t index = 0; index < 10; ++index)
				{
					const http::Response& response = service->request(request).get();
					if (response.error_code != http::ErrorCode::OK)
						return "Unexpected error_code: " + std::to_string(static_cast<int>(response.error_code));
					else if (response.status_code != http::Status::OK)
						return "Unexpected http status: " + std::to_string(static_cast<int>(response.status_code));
				}
				const auto end = std::chrono::system_clock::now();
				sync_time = end - start;
			}

			std::chrono::system_clock::duration async_time;
			{
				std::vector<std::future<http::Response>> responses;
				responses.resize(10);
				const auto start = std::chrono::system_clock::now();
				for (std::size_t index = 0; index < 10; ++index)
					responses[index] = service->request(request);
				for (std::size_t index = 0; index < 10; ++index)
				{
					const http::Response& response = responses[index].get();
					if (response.error_code != http::ErrorCode::OK)
						return "Unexpected error_code: " + std::to_string(static_cast<int>(response.error_code));
					else if (response.status_code != http::Status::OK)
						return "Unexpected http status: " + std::to_string(static_cast<int>(response.status_code));
				}
				const auto end = std::chrono::system_clock::now();
				async_time = end - start;
			}

			if ( 4.5f * async_time > sync_time)
			{
				std::stringstream stream;
				stream << "Rate: " << static_cast<float>(sync_time.count())/async_time.count();
				return "Async requests are too slow. " + stream.str();
			}

			return "";
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
};

