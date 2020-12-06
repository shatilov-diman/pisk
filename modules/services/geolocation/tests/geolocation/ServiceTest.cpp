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

#include "../../sources/geolocation/ServiceImpl.h"

using namespace igloo;
using namespace pisk;
using namespace pisk::services::geolocation;

ServicePtr CreateServiceForTest(Providers&& providers);

template<Provider provider>
class TestProvider :
	public GeolocationProvider
{
public:
	utils::auto_unsubscriber subscription;
	bool enabled_locate = false;

	explicit TestProvider(LocationSignaler& _signaler)
	{
		subscription = _signaler.subscribe([this](const Location& location) {
			on_update_location.emit(location);
		});
	}

	virtual bool start_locate() final override
	{
		enabled_locate = true;
		return true;
	}

	virtual bool stop_locate() final override
	{
		enabled_locate = false;
		return true;
	}
};

template <Provider provider_type>
static GeolocationProviderPtr make_test_provider(LocationSignaler& _signaler)
{
	return std::make_unique<TestProvider<provider_type>>(_signaler);
}

Describe(GeoLocation) {
	std::chrono::system_clock::time_point now_time = std::chrono::system_clock::now();
	Context(no_providers) {
		tools::InterfacePtr<ServiceImpl> service;
		void SetUp() {
			Providers providers;
			service = CreateServiceForTest(std::move(providers)).cast<ServiceImpl>();
		}
		Spec(no_available_providers) {
			Assert::That(service->get_available_providers().empty(), Is().EqualTo(true));
		}
		Spec(unable_to_enable_disable_providers) {
			Assert::That(service->enable_provider(Provider::None), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::IP), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::PhoneCell), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::Sattelite), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::Manual), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::None), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::IP), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::PhoneCell), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::Sattelite), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::Manual), Is().EqualTo(false));
		}
		Spec(timestamp_is_default) {
			Assert::That(service->get_location().timestamp, Is().EqualTo(std::chrono::system_clock::time_point{}));
		}
		Spec(location_is_none) {
			Assert::That(service->get_location().provider, Is().EqualTo(Provider::None));
		}
	};
	Context(one_provider) {
		tools::InterfacePtr<ServiceImpl> service;
		LocationSignaler update_location;
		void SetUp() {
			Providers providers;
			providers[Provider::IP] = make_test_provider<Provider::IP>(update_location);
			service = CreateServiceForTest(std::move(providers)).cast<ServiceImpl>();
		}
		Spec(IP_available_provider) {
			Assert::That(service->get_available_providers(), Is().EqualTo(std::vector<Provider>{Provider::IP}));
		}
		Spec(available_enable_disable_providers) {
			Assert::That(service->enable_provider(Provider::IP), Is().EqualTo(true));
			Assert::That(service->disable_provider(Provider::IP), Is().EqualTo(true));
		}
		Spec(unable_to_enable_provider_twice) {
			Assert::That(service->enable_provider(Provider::IP), Is().EqualTo(true));
			Assert::That(service->enable_provider(Provider::IP), Is().EqualTo(false));
		}
		Spec(unable_to_disable_provider_without_enable_it) {
			Assert::That(service->disable_provider(Provider::IP), Is().EqualTo(false));
		}
		Spec(unable_to_disable_provider_twice) {
			Assert::That(service->enable_provider(Provider::IP), Is().EqualTo(true));
			Assert::That(service->disable_provider(Provider::IP), Is().EqualTo(true));
			Assert::That(service->disable_provider(Provider::IP), Is().EqualTo(false));
		}
		Spec(unable_to_enable_disable_providers) {
			Assert::That(service->enable_provider(Provider::None), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::PhoneCell), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::Sattelite), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::Manual), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::None), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::PhoneCell), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::Sattelite), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::Manual), Is().EqualTo(false));
		}
		Spec(location_is_none) {
			Assert::That(service->get_location().timestamp, Is().EqualTo(std::chrono::system_clock::time_point{}));
			Assert::That(service->get_location().provider, Is().EqualTo(Provider::None));
		}
		When(provider_emit_update) {
			const Location loc_now {Root().now_time, 50000, 1.13, 44.15, Provider::IP};
			void SetUp() {
				Parent().service->enable_provider(Provider::IP);
				Parent().update_location.emit(loc_now);
			}
			Then(location_is_expected) {
				Assert::That(Parent().service->get_location(), Is().EqualTo(loc_now));
			}
			When(provider_emit_old_update) {
				const Location loc_old {Root().now_time-std::chrono::minutes(1), 50000, 1.13, 44.15, Provider::IP};
				void SetUp() {
					Parent().Parent().update_location.emit(loc_old);
				}
				Then(location_is_expected) {
					Assert::That(Parent().Parent().service->get_location(), Is().EqualTo(Parent().loc_now));
				}
			};
			When(provider_emit_new_update) {
				const Location loc_new {Root().now_time+std::chrono::minutes(1), 50000, 1.13, 44.15, Provider::IP};
				void SetUp() {
					Parent().Parent().update_location.emit(loc_new);
				}
				Then(location_is_expected) {
					Assert::That(Parent().Parent().service->get_location(), Is().EqualTo(loc_new));
				}
			};
		};
	};
	Context(two_providers) {
		tools::InterfacePtr<ServiceImpl> service;
		LocationSignaler web_update_location;
		LocationSignaler phone_update_location;
		void SetUp() {
			Providers providers;
			providers[Provider::IP] = make_test_provider<Provider::IP>(web_update_location);
			providers[Provider::PhoneCell] = make_test_provider<Provider::PhoneCell>(phone_update_location);
			service = CreateServiceForTest(std::move(providers)).cast<ServiceImpl>();
		}
		Spec(service_available_providers) {
			Assert::That(service->get_available_providers(), Is().EqualTo(std::vector<Provider>{Provider::IP, Provider::PhoneCell}));
		}
		Spec(available_enable_disable_providers) {
			Assert::That(service->enable_provider(Provider::IP), Is().EqualTo(true));
			Assert::That(service->enable_provider(Provider::PhoneCell), Is().EqualTo(true));
			Assert::That(service->disable_provider(Provider::IP), Is().EqualTo(true));
			Assert::That(service->disable_provider(Provider::PhoneCell), Is().EqualTo(true));
		}
		Spec(unable_to_enable_disable_providers) {
			Assert::That(service->enable_provider(Provider::None), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::Sattelite), Is().EqualTo(false));
			Assert::That(service->enable_provider(Provider::Manual), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::None), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::Sattelite), Is().EqualTo(false));
			Assert::That(service->disable_provider(Provider::Manual), Is().EqualTo(false));
		}
		Spec(location_timestamp_is_default) {
			Assert::That(service->get_location().timestamp, Is().EqualTo(std::chrono::system_clock::time_point{}));
		}
		Spec(location_is_none) {
			Assert::That(service->get_location().provider, Is().EqualTo(Provider::None));
		}
		When(provider_emit_update_web) {
			const Location loc_now {Root().now_time, 50000, 1.13, 44.15, Provider::IP};
			void SetUp() {
				Parent().service->enable_provider(Provider::IP);
				Parent().service->enable_provider(Provider::PhoneCell);
				Parent().web_update_location.emit(loc_now);
			}
			Then(location_is_expected) {
				Assert::That(Parent().service->get_location(), Is().EqualTo(loc_now));
			}
			When(provider_emit_update_phone_old) {
				const Location loc_old {Root().now_time-std::chrono::minutes(1), 50000, 1.13, 44.15, Provider::PhoneCell};
				void SetUp() {
					Parent().Parent().phone_update_location.emit(loc_old);
				}
				Then(location_is_expected) {
					Assert::That(Parent().Parent().service->get_location(), Is().EqualTo(loc_old));
				}
			};
			When(provider_emit_update_phone_very_old) {
				const Location loc_old {Root().now_time-std::chrono::minutes(7), 50000, 1.12, 43.15, Provider::PhoneCell};
				void SetUp() {
					Parent().Parent().phone_update_location.emit(loc_old);
				}
				Then(location_is_expected) {
					Assert::That(Parent().Parent().service->get_location(), Is().EqualTo(Parent().loc_now));
				}
			};
			When(provider_emit_update_phone_new) {
				const Location loc_new {Root().now_time+std::chrono::minutes(1), 50000, 1.11, 42.15, Provider::PhoneCell};
				void SetUp() {
					Parent().Parent().phone_update_location.emit(loc_new);
				}
				Then(location_is_expected) {
					Assert::That(Parent().Parent().service->get_location(), Is().EqualTo(loc_new));
				}
			};
		};
		When(provider_emit_update_phone) {
			const Location loc_now {Root().now_time, 50000, 1.13, 44.15, Provider::PhoneCell};
			void SetUp() {
				Parent().service->enable_provider(Provider::IP);
				Parent().service->enable_provider(Provider::PhoneCell);
				Parent().phone_update_location.emit(loc_now);
			}
			Then(location_is_expected) {
				Assert::That(Parent().service->get_location(), Is().EqualTo(loc_now));
			}
			When(provider_emit_update_web_old) {
				const Location loc_old {Root().now_time-std::chrono::minutes(1), 50000, 1.13, 44.15, Provider::IP};
				void SetUp() {
					Parent().Parent().web_update_location.emit(loc_old);
				}
				Then(location_is_expected) {
					Assert::That(Parent().Parent().service->get_location(), Is().EqualTo(Parent().loc_now));
				}
			};
			When(provider_emit_update_web_new) {
				const Location loc_new {Root().now_time+std::chrono::minutes(1), 50000, 1.11, 42.15, Provider::IP};
				void SetUp() {
					Parent().Parent().web_update_location.emit(loc_new);
				}
				Then(location_is_expected) {
					Assert::That(Parent().Parent().service->get_location(), Is().EqualTo(Parent().loc_now));
				}
			};
			When(provider_emit_update_web_very_new) {
				const Location loc_new {Root().now_time+std::chrono::minutes(7), 50000, 1.12, 43.15, Provider::IP};
				void SetUp() {
					Parent().Parent().web_update_location.emit(loc_new);
				}
				Then(location_is_expected) {
					Assert::That(Parent().Parent().service->get_location(), Is().EqualTo(loc_new));
				}
			};
		};
	};
};


