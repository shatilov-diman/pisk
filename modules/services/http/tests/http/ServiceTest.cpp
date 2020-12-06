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

#include "../../sources/http/ServiceImpl.h"
#include "../../sources/http/CurlWorker.h"

#include <thread>
#include <chrono>

static const char test_host[] = "httpbin.org";

using namespace igloo;
using namespace pisk::services::http;

ServicePtr CreateHttpServiceForTest()
{
	auto worker = std::make_unique<cURLWorker>();
	auto service = std::make_shared<ServiceImpl>(std::move(worker));
	return {{}, service};
}


Describe(HttpService) {
	ServicePtr service;
	void SetUp() {
		service = CreateHttpServiceForTest();
	}
	void TearDown()
	{
		service.reset();
	}
	It(happy_day_scenario) {
		Request request {
			{"http", "", test_host, "", "/", "", ""},
			{},
			{},
			Method::GET
		};
		const Response& response = service->request(request).get();
		Assert::That(response.error_code, Is().EqualTo(ErrorCode::OK));
		Assert::That(response.status_code, Is().EqualTo(Status::OK));
	}
	It(incorrect_url_error) {
		Request request {
			{"XXX", "", test_host, "", "/", "", ""},
			{},
			{},
			Method::GET
		};
		const Response& response = service->request(request).get();
		Assert::That(response.error_code, Is().Not().EqualTo(ErrorCode::OK));
		Assert::That(response.status_code, Is().EqualTo(Status::UNKNOWN));
	}
	It(unreachable_url_error) {
		Request request {
			{"", "", "yasdvnlawrg04t.ru", "", "/", "", ""},
			{},
			{},
			Method::GET
		};
		const Response& response = service->request(request).get();
		Assert::That(response.error_code, Is().Not().EqualTo(ErrorCode::OK));
		Assert::That(response.status_code, Is().EqualTo(Status::UNKNOWN));
	}
	It(http_404) {
		Request request {
			{"http", "", test_host, "", "/qweasdzxc", "", ""},
			{},
			{},
			Method::GET
		};
		const Response& response = service->request(request).get();
		Assert::That(response.error_code, Is().EqualTo(ErrorCode::OK));
		Assert::That(response.status_code, Is().EqualTo(Status::NOT_FOUND));
	}
	It(parallel_requests) {
		Request request_1 {
			{ "http", "", test_host, "", "/delay/3", "", "" },
			{},
			{},
			Method::GET,
			reinterpret_cast<void*>(13)
		};
		Request request_2 {
			{ "http", "", test_host, "", "/delay/1", "", "" },
			{},
			{},
			Method::GET,
			reinterpret_cast<void*>(14)
		};
		auto future_1 = service->request(request_1);
		auto future_2 = service->request(request_2);

		const auto& response_2 = future_2.get();
		Assert::That(response_2.error_code, Is().EqualTo(ErrorCode::OK));
		Assert::That(response_2.status_code, Is().EqualTo(Status::OK));
		Assert::That(response_2.request.userdata, Is().EqualTo(reinterpret_cast<void*>(14)));

		const auto& response_1 = future_1.get();
		Assert::That(response_1.error_code, Is().EqualTo(ErrorCode::OK));
		Assert::That(response_1.status_code, Is().EqualTo(Status::OK));
		Assert::That(response_1.request.userdata, Is().EqualTo(reinterpret_cast<void*>(13)));
	}
};

Describe(build_path_from_URL)
{
	URL url;
	Context(very_simple_request) {
		void SetUp() {
			Root().url = URL {"", "", "ya.ru", "", "", "", ""};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("ya.ru"));
		}
		Spec(request_part_is_empty){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo(""));
		}
		Spec(url_is_http_yaru){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("ya.ru"));
		}
	};
	Context(simple_request) {
		void SetUp() {
			Root().url = URL {"http", "", "ya.ru", "", "", "", ""};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("http://ya.ru"));
		}
		Spec(request_part_is_empty){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo(""));
		}
		Spec(url_is_http_yaru){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("http://ya.ru"));
		}
	};
	Context(request_with_port) {
		void SetUp() {
			Root().url = URL {"https", "", "ya.ru", "8080", "", "", ""};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("https://ya.ru:8080"));
		}
		Spec(request_part_is_empty){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo(""));
		}
		Spec(url_is_http_yaru){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("https://ya.ru:8080"));
		}
	};
	Context(authorize_with_password) {
		void SetUp() {
			Root().url = URL {"ftp", "user:password", "ya.ru", "", "", "", ""};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("ftp://user:password@ya.ru"));
		}
		Spec(request_part_is_empty){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo(""));
		}
		Spec(url_is_http_auth_yaru){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("ftp://user:password@ya.ru"));
		}
	};
	Context(url_with_path) {
		void SetUp() {
			Root().url = URL {"http", "", "ya.ru", "", "/internet.txt", "", ""};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("http://ya.ru"));
		}
		Spec(request_part_is_slash){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo("/internet.txt"));
		}
		Spec(url_is_http_yaru_slash){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("http://ya.ru/internet.txt"));
		}
	};
	Context(url_with_path_with_query) {
		void SetUp() {
			Root().url = URL {"http", "", "ya.ru", "", "/internet.txt", "qwe=123&zcx=456", ""};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("http://ya.ru"));
		}
		Spec(request_part_is_internet_with_query){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo("/internet.txt?qwe=123&zcx=456"));
		}
		Spec(url_is_http_yaru_slash){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("http://ya.ru/internet.txt?qwe=123&zcx=456"));
		}
	};
	Context(url_with_path_with_fragment) {
		void SetUp() {
			Root().url = URL {"http", "", "ya.ru", "", "/internet.txt", "", "frag"};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("http://ya.ru"));
		}
		Spec(request_part_is_internet_with_fragment){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo("/internet.txt#frag"));
		}
		Spec(url_is_http_yaru_slash){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("http://ya.ru/internet.txt#frag"));
		}
	};
	Context(url_with_path_with_query_and_fragment) {
		void SetUp() {
			Root().url = URL {"http", "", "ya.ru", "", "/internet.txt", "qwe=123&zcx=456", "frag"};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("http://ya.ru"));
		}
		Spec(request_part_is_internet_with_query){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo("/internet.txt?qwe=123&zcx=456#frag"));
		}
		Spec(url_is_http_yaru_slash){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("http://ya.ru/internet.txt?qwe=123&zcx=456#frag"));
		}
	};
	Context(url_with_fragment) {
		void SetUp() {
			Root().url = URL {"http", "", "ya.ru", "", "", "", "frag"};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("http://ya.ru"));
		}
		Spec(request_part_is_internet_with_query){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo("#frag"));
		}
		Spec(url_is_http_yaru_slash){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("http://ya.ru#frag"));
		}
	};
	Context(url_with_root_path_with_query_and_fragment) {
		void SetUp() {
			Root().url = URL {"http", "", "ya.ru", "", "/", "qwe=123&zcx=456", "frag"};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("http://ya.ru"));
		}
		Spec(request_part_is_internet_with_query){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo("/?qwe=123&zcx=456#frag"));
		}
		Spec(url_is_http_yaru_slash){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("http://ya.ru/?qwe=123&zcx=456#frag"));
		}
	};
	Context(url_all_fields) {
		void SetUp() {
			Root().url = URL {"https", "user:password", "ya.ru", "8080", "/internet.txt", "qwe=123&zcx=456", "frag"};
		}
		Spec(domain_part_is_http_yaru){
			Assert::That(build_domain_path(Root().url).get_content(), Is().EqualTo("https://user:password@ya.ru:8080"));
		}
		Spec(request_part_is_internet_with_query){
			Assert::That(build_request_path(Root().url).get_content(), Is().EqualTo("/internet.txt?qwe=123&zcx=456#frag"));
		}
		Spec(url_is_http_yaru_slash){
			Assert::That(to_string(Root().url).get_content(), Is().EqualTo("https://user:password@ya.ru:8080/internet.txt?qwe=123&zcx=456#frag"));
		}
	};
};



Describe(build_URL_from_path)
{
	Context(empty_request) {
		Spec(url_is_expected){
			URL url = to_url("");
			Assert::That(url, Is().EqualTo(URL {"", "", "", "", "", "", ""}));
		}
	};
	Context(very_simple_request) {
		Spec(url_is_expected){
			URL url = to_url("ya.ru");
			Assert::That(url, Is().EqualTo(URL {"", "", "ya.ru", "", "", "", ""}));
		}
	};
	Context(request_with_protocol) {
		Spec(url_is_expected){
			URL url = to_url("https://ya.ru");
			Assert::That(url, Is().EqualTo(URL {"https", "", "ya.ru", "", "", "", ""}));
		}
	};
	Context(request_with_protocol_user_and_password) {
		Spec(url_is_expected){
			URL url = to_url("ftp://user:password@ya.ru");
			Assert::That(url, Is().EqualTo(URL {"ftp", "user:password", "ya.ru", "", "", "", ""}));
		}
	};
	Context(request_with_protocol_user_and_password_and_port) {
		Spec(url_is_expected){
			URL url = to_url("ftp://user:password@ya.ru:8000");
			Assert::That(url, Is().EqualTo(URL {"ftp", "user:password", "ya.ru", "8000", "", "", ""}));
		}
	};
	Context(request_with_path) {
		Spec(url_is_expected){
			URL url = to_url("ya.ru/internet.txt");
			Assert::That(url, Is().EqualTo(URL {"", "", "ya.ru", "", "/internet.txt", "", ""}));
		}
	};
	Context(request_with_port_and_root_path) {
		Spec(url_is_expected){
			URL url = to_url("ya.ru:8080/");
			Assert::That(url, Is().EqualTo(URL {"", "", "ya.ru", "8080", "/", "", ""}));
		}
	};
	Context(request_with_port_and_path) {
		Spec(url_is_expected){
			URL url = to_url("http://ya.ru:8080/internet.txt");
			Assert::That(url, Is().EqualTo(URL {"http", "", "ya.ru", "8080", "/internet.txt", "", ""}));
		}
	};
	Context(request_with_query) {
		Spec(url_is_expected){
			URL url = to_url("ya.ru:8080?qwe=123&zcx=456");
			Assert::That(url, Is().EqualTo(URL {"", "", "ya.ru", "8080", "", "qwe=123&zcx=456", ""}));
		}
	};
	Context(request_with_fragment) {
		Spec(url_is_expected){
			URL url = to_url("ya.ru:8080#frag");
			Assert::That(url, Is().EqualTo(URL {"", "", "ya.ru", "8080", "", "", "frag"}));
		}
	};
	Context(request_with_query_without_pirt) {
		Spec(url_is_expected){
			URL url = to_url("ya.ru?qwe=123&zcx=456");
			Assert::That(url, Is().EqualTo(URL {"", "", "ya.ru", "", "", "qwe=123&zcx=456", ""}));
		}
	};
	Context(request_with_fragment_without_pirt) {
		Spec(url_is_expected){
			URL url = to_url("ya.ru#frag");
			Assert::That(url, Is().EqualTo(URL {"", "", "ya.ru", "", "", "", "frag"}));
		}
	};
	Context(all_fields) {
		Spec(url_is_expected){
			URL url = to_url("https://user:password@ya.ru:8080/internet.txt?qwe=123&zcx=456#frag");
			Assert::That(url, Is().EqualTo(URL {"https", "user:password", "ya.ru", "8080", "/internet.txt", "qwe=123&zcx=456", "frag"}));
		}
	};
};


Describe(test_build_query) {
	Spec(empty_is_expected){
		const auto& query = build_query({{}});
		Assert::That(query, Is().EqualTo(""));
	}
	Spec(empty_is_expected_when_key_empty){
		const auto& query = build_query({{"", "q"}});
		Assert::That(query, Is().EqualTo(""));
	}
	Spec(empty_is_expected_when_both_empty){
		const auto& query = build_query({{"", ""}});
		Assert::That(query, Is().EqualTo(""));
	}
	Spec(key_with_empty_value_is_expected_when_value_empty){
		const auto& query = build_query({{"k", ""}});
		Assert::That(query, Is().EqualTo("k="));
	}
	Spec(empty_is_expected_when_key_empty_2){
		const auto& query = build_query({{"", "q"}, {"a", "b"}});
		Assert::That(query, Is().EqualTo("a=b"));
	}
	Spec(empty_is_expected_when_both_empty_2){
		const auto& query = build_query({{"", ""}, {"a", "b"}});
		Assert::That(query, Is().EqualTo("a=b"));
	}
	Spec(key_with_empty_value_is_expected_when_value_empty_2){
		const auto& query = build_query({{"k", ""}, {"a", "b"}});
		Assert::That(query, Is().EqualTo("k=&a=b"));
	}
	Spec(key_with_empty_value_is_expected_when_value_empty_3){
		const auto& query = build_query({{"a", "b"}, {"k", ""}});
		Assert::That(query, Is().EqualTo("a=b&k="));
	}
};

