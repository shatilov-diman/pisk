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
#include <pisk/infrastructure/Module.h>
#include <pisk/tools/Application.h>

#include <pisk/system/ResourceManager.h>
#include <pisk/tools/MainLoop.h>

#include <stdexcept>

#include "ServiceTest.h"

using namespace igloo;

extern pisk::infrastructure::ModulePtr CreateModule(const std::string& basename);

Context(check_Application_api) {
	Describe(check_system_modules) {
		Spec(ResourceManager_available_while_process_configurator_components_list_provider) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			app->run({
				make_test_components(),
				[](const pisk::tools::ServiceRegistry& sl) -> void {
					Assert::That(sl.get<pisk::system::ResourceManager>(), Is().Not().EqualTo(nullptr));
				},
				[](const pisk::tools::ServiceRegistry& sl) -> pisk::tools::components::DescriptionsList {
					Assert::That(sl.get<pisk::system::ResourceManager>(), Is().Not().EqualTo(nullptr));
					Assert::That(sl.get<pisk::tools::MainLoop>(), Is().Not().EqualTo(nullptr));
					return {};
				},
				[](const pisk::tools::ServiceRegistry& sl) -> void {
					sl.get<pisk::tools::MainLoop>()->stop();
					Assert::That(sl.get<pisk::system::ResourceManager>(), Is().Not().EqualTo(nullptr));
					Assert::That(sl.get<pisk::tools::MainLoop>(), Is().Not().EqualTo(nullptr));
					Assert::That(sl.get<TestOsAppInstnace>(), Is().Not().EqualTo(nullptr));
				},
				&CreateModule,
			});
		}
	};
	When(exception_thrown_while_make_os_component) {
		Then(exception_forward_out_andapplication_terminating) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(std::domain_error, app->run({
				{{"yyy", [](const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory&, const pisk::utils::property&)->pisk::tools::SafeComponentPtr
					{throw std::domain_error("yyy");}
				}},
				[](const pisk::tools::ServiceRegistry&) -> void {},
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					return {};
				},
				[](const pisk::tools::ServiceRegistry&) -> void { },
				&CreateModule,
			}));
		}
	};
	When(exception_thrown_while_process_configuration_configure_core_components) {
		Then(exception_forward_out_andapplication_terminating) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(std::domain_error, app->run({
				make_test_components(),
				[](const pisk::tools::ServiceRegistry&) -> void {
					throw std::domain_error("xxx");
				},
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					return {};
				},
				[](const pisk::tools::ServiceRegistry&) -> void { },
				&CreateModule,
			}));
		}
	};
	When(exception_thrown_while_process_configuration_components_list_provider) {
		Then(exception_forward_out_andapplication_terminating) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(std::domain_error, app->run({
				make_test_components(),
				[](const pisk::tools::ServiceRegistry&) -> void { },
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					throw std::domain_error("xxx");
					return {};
				},
				[](const pisk::tools::ServiceRegistry&) -> void { },
				&CreateModule,
			}));
		}
	};
	When(exception_thrown_while_process_configuration_configure_components) {
		Then(exception_forward_out_andapplication_terminating) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(std::domain_error, app->run({
				make_test_components(),
				[](const pisk::tools::ServiceRegistry&) -> void { },
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					return {};
				},
				[](const pisk::tools::ServiceRegistry&) -> void {
					throw std::domain_error("xxx");
				},
				&CreateModule,
			}));
		}
	};
	When(exception_thrown_while_load_module) {
		Then(exception_forward_out_andapplication_terminating) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(std::domain_error, app->run({
				make_test_components(),
				[](const pisk::tools::ServiceRegistry&) -> void { },
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					return {};
				},
				[](const pisk::tools::ServiceRegistry&) -> void { },
				[](const std::string&) -> pisk::infrastructure::ModulePtr {
					throw std::domain_error("xxx");
				},
			}));
		}
	};
	When(configuration_configure_components_is_null) {
		Then(application_runs) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(pisk::infrastructure::NullPointerException, app->run({
				make_test_components(),
				[](const pisk::tools::ServiceRegistry&) -> void { },
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					return {};
				},
				nullptr,
				&CreateModule,
			}));
		}
	};
	When(configurator_components_list_provider_is_null) {
		Then(exception_forward_out_andapplication_terminating) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(pisk::infrastructure::NullPointerException, app->run({
				make_test_components(),
				[](const pisk::tools::ServiceRegistry&) -> void { },
				nullptr,
				[](const pisk::tools::ServiceRegistry&) -> void {},
				&CreateModule,
			}));
		}
	};
	When(configuration_configure_core_components_is_null) {
		Then(application_runs) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(pisk::infrastructure::NullPointerException, app->run({
				make_test_components(),
				nullptr,
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					return {};
				},
				[](const pisk::tools::ServiceRegistry&) -> void { },
				&CreateModule,
			}));
		}
	};
	When(configuration_make_os_component_is_nullptr) {
		Then(application_runs) {
			auto os_components = make_test_components();
			os_components.push_back({"yyy", [](const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory&, const pisk::utils::property&) -> pisk::tools::SafeComponentPtr
				{return {};}
			});
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			app->run({
				os_components,
				[](const pisk::tools::ServiceRegistry&) -> void { },
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					return {};
				},
				[](const pisk::tools::ServiceRegistry& sl) -> void {
					sl.get<pisk::tools::MainLoop>()->stop();
					Assert::That(sl.get<pisk::core::Component>("yyy"), Is().EqualTo(nullptr));
					Assert::That(sl.get<pisk::core::Component>("zzz"), Is().EqualTo(nullptr));
				},
				&CreateModule,
			});
		}
	};
	When(configuration_module_loader_is_null) {
		Then(application_runs) {
			pisk::tools::ApplicationPtr app = pisk::tools::make_application();
			AssertThrows(pisk::infrastructure::NullPointerException, app->run({
				make_test_components(),
				[](const pisk::tools::ServiceRegistry&) -> void { },
				[](const pisk::tools::ServiceRegistry&) -> pisk::tools::components::DescriptionsList {
					return {};
				},
				[](const pisk::tools::ServiceRegistry&) -> void { },
				nullptr,
			}));
		}
	};
};

