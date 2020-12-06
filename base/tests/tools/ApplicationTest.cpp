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
#include <pisk/tools/MainLoop.h>
#include <pisk/tools/Application.h>
#include <pisk/tools/OsAppInstance.h>
#include <pisk/tools/ServiceRegistry.h>

#include <functional>
#include <atomic>
#include <array>

using namespace igloo;
using namespace pisk::tools;
using namespace pisk::tools::components;

class TestMainLoop : public MainLoop
{
	std::atomic_bool loop;

	virtual void release() final override {
		delete this;
	}
	virtual void run() override {
		loop = true;
	}
	virtual void stop() override {
		loop = false;
	}
};
class TestMainLoopThrowException : public TestMainLoop {
	virtual void run() final override {
		throw pisk::infrastructure::Exception();
	}
};

class TestMainLoopThrowUnknownException : public TestMainLoop {
	virtual void run() final override {
		throw 42;
	}
};

template <ComponentFactory factory>
ComponentFactory factory_getter(){
	return factory;
}

class TestModule : public pisk::infrastructure::Module {
	template <typename MainLoop>
	static SafeComponentPtr __cdecl test_main_loop_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property&) {
		return factory.make<MainLoop>();
	}
	template <typename MainLoop>
	static ComponentFactory __cdecl test_main_loop_factory_getter() {
		return &test_main_loop_factory<MainLoop>;
	}
	virtual ProcedurePtr find_procedure(const std::string& name) final override {
		if (name == "test_main_loop_factory")
			return reinterpret_cast<ProcedurePtr>(&test_main_loop_factory_getter<TestMainLoop>);
		if (name == "test_main_loop_throw_exception_factory")
			return reinterpret_cast<ProcedurePtr>(&test_main_loop_factory_getter<TestMainLoopThrowException>);
		if (name == "test_main_loop_throw_unknown_exception_factory")
			return reinterpret_cast<ProcedurePtr>(&test_main_loop_factory_getter<TestMainLoopThrowUnknownException>);
		return nullptr;
	}
	virtual void release() final override { delete this; }
};

class TestModuleLoader
{
public:
	static pisk::infrastructure::ModulePtr module_loader(const std::string&) {
		return make_shared_releasable<TestModule>();
	}
};

enum test_event_type
{
	InitApp,
	DeinitApp,
	KeyDown,
	KeyUp,
};

class OsAppInstanceImpl :
	public OsAppInstance
{
	virtual void release() final override
	{
		delete this;
	}
public:
	explicit OsAppInstanceImpl(const int value):
		check_value(value)
	{}

	const int check_value = 0;
};

class TestApplication
{
	using ModuleLoaderFn = std::function <pisk::infrastructure::ModulePtr (const std::string& basename)>;
	ModuleLoaderFn module_loader;
	pisk::tools::unique_relesable_ptr<Application> impl;

	std::atomic_int step;
public:
	pisk::utils::signaler<void> on_configure;

	explicit TestApplication(const ModuleLoaderFn& module_loader) :
		module_loader(module_loader),
		impl(pisk::tools::make_application())
	{
		step = -1;
	}

	void run()
	{
		const components::DescriptionsList test_components_desc = components::DescriptionsList {
		//	components::Description {"service", "logger", "system", "logger_factory"},
			components::Description {"service", pisk::tools::MainLoop::uid, "", "test_main_loop_factory"},
		};
		run(test_components_desc);
	}
	void run(const pisk::tools::components::DescriptionsList& descriptions)
	{
		OsAppInstanceImpl os_app_instance(41);

		const pisk::tools::OsComponentList components {
			{
				OsAppInstanceImpl::uid,
				[](const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&) -> pisk::tools::SafeComponentPtr {
					return factory.make<OsAppInstanceImpl>(42);
				}
			}
		};

		impl->run({
			components,
			[](const pisk::tools::ServiceRegistry&) -> void { },
			[descriptions](const pisk::tools::ServiceRegistry&) -> components::DescriptionsList {
				return descriptions;
			},
			[this](const pisk::tools::ServiceRegistry&) -> void {
				on_configure.emit();
			},
			module_loader
		});
	}

private:
	pisk::utils::auto_unsubscriber app_start;
	pisk::utils::auto_unsubscriber app_stop;
	pisk::utils::auto_unsubscriber win_init;
	pisk::utils::auto_unsubscriber win_deinit;
	pisk::utils::auto_unsubscriber key_down;
	pisk::utils::auto_unsubscriber key_up;
};

template <typename ModuleLoader>
class TestApplicationEx :
	public TestApplication
{
public:
	TestApplicationEx():
		TestApplication(
			&ModuleLoader::module_loader
		)
	{}
};

Describe(ApplicationTest) {
	It(common_usage) {
		TestApplicationEx<TestModuleLoader> app;
		app.run();
	}

	When(factory_does_not_provide_main_loop) {
		const components::DescriptionsList test_components_desc = components::DescriptionsList {
			components::Description {"service", pisk::tools::MainLoop::uid, "", "no_main_loop_factory"},
		};
		Then(no_run) {
			TestApplicationEx<TestModuleLoader> app;
			AssertThrows(pisk::infrastructure::NullPointerException,
				app.run(test_components_desc));
		}
	};

	When(run_throw_exception) {
		const components::DescriptionsList test_components_desc = components::DescriptionsList {
			components::Description {"service", pisk::tools::MainLoop::uid, "", "test_main_loop_throw_exception_factory"},
		};
		Then(no_deinit_event) {
			TestApplicationEx<TestModuleLoader> app;
			AssertThrows(pisk::infrastructure::Exception,
				app.run(test_components_desc));
		}
	};
	When(run_throw_unknown_exception) {
		const components::DescriptionsList test_components_desc = components::DescriptionsList {
			components::Description {"service", pisk::tools::MainLoop::uid, "", "test_main_loop_throw_unknown_exception_factory"},
		};
		Then(no_deinit_event) {
			TestApplicationEx<TestModuleLoader> app;
			AssertThrows(int, app.run(test_components_desc));
		}
	};
};

struct SLTestComponent : public pisk::core::Component {
	constexpr static const char* uid = "ttt";

	virtual void release() final override { delete this; }
};
struct SLTestModule : public pisk::infrastructure::Module {
	static SafeComponentPtr __cdecl test_main_loop_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&) {
		auto app = temp_sl.get<OsAppInstanceImpl>();
		Assert::That(app, Is().Not().EqualTo(nullptr));
		Assert::That(static_cast<const OsAppInstanceImpl&>(*app).check_value, Is().EqualTo(42));
		return factory.make<TestMainLoop>();
	}
	static SafeComponentPtr __cdecl sl_test_nullptr_factory(const ServiceRegistry&, const InstanceFactory&, const pisk::utils::property&) {
		return {};
	}
	static SafeComponentPtr __cdecl sl_test_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property&) {
		return factory.make<SLTestComponent>();
	}
	static SafeComponentPtr __cdecl sl_check_test_not_found__factory(const ServiceRegistry& temp_sl, const InstanceFactory&, const pisk::utils::property&) {
		auto component = temp_sl.get<SLTestComponent>();
		Assert::That(component, Is().EqualTo(nullptr));
		return {};
	}
	static SafeComponentPtr __cdecl sl_check_test_found__factory(const ServiceRegistry& temp_sl, const InstanceFactory&, const pisk::utils::property&) {
		auto component = temp_sl.get<SLTestComponent>();
		Assert::That(component, Is().Not().EqualTo(nullptr));
		return {};
	}
	virtual ProcedurePtr find_procedure(const std::string& name) final override {
		if (name == "test_main_loop_factory")
			return reinterpret_cast<ProcedurePtr>(&factory_getter<&test_main_loop_factory>);
		if (name == "sl_test_nullptr_factory")
			return reinterpret_cast<ProcedurePtr>(&factory_getter<&sl_test_nullptr_factory>);
		if (name == "sl_test_factory")
			return reinterpret_cast<ProcedurePtr>(&factory_getter<&sl_test_factory>);
		if (name == "sl_check_test_not_found__factory")
			return reinterpret_cast<ProcedurePtr>(&factory_getter<&sl_check_test_not_found__factory>);
		if (name == "sl_check_test_found__factory")
			return reinterpret_cast<ProcedurePtr>(&factory_getter<&sl_check_test_found__factory>);
		return nullptr;
	}
	virtual void release() final override { delete this; }
};
class SLTestModuleLoader {
public:
	static pisk::infrastructure::ModulePtr module_loader(const std::string&) {
		return make_shared_releasable<SLTestModule>();
	}
};
Describe(ApplicationServiceRegistryTest) {
	std::unique_ptr<TestApplicationEx<SLTestModuleLoader>> app;
	void SetUp() {
		app = std::make_unique<TestApplicationEx<SLTestModuleLoader>>();
	}
	When(factory_returns_nullptr) {
		Then(app_sl_returns_nullptr) {
			Root().app->run(pisk::tools::components::DescriptionsList{
				{"service", pisk::tools::MainLoop::uid, "", "test_main_loop_factory"},
				{"tst", SLTestComponent::uid, "test_sl", "sl_test_nullptr_factory"},
				{"tst", "testname", "test_sl", "sl_check_test_not_found__factory"},
			});
		}
	};
	When(factory_returns_component) {
		Then(app_sl_returns_component) {
			Root().app->run(pisk::tools::components::DescriptionsList{
				{"service", pisk::tools::MainLoop::uid, "", "test_main_loop_factory"},
				{"tst", SLTestComponent::uid, "test_sl", "sl_test_factory"},
				{"tst", "testname", "test_sl", "sl_check_test_found__factory"},
			});
		}
	};
};

static std::size_t test_order_load_index = 0;
static std::size_t fail_load_index = 0;

struct OrderTestComponent : public pisk::core::Component {
	virtual void release() final override { delete this; }
};
struct TestModuleOrder : public pisk::infrastructure::Module {
	const std::size_t order_index;
	TestModuleOrder() :
		order_index(++test_order_load_index)
	{}
	static SafeComponentPtr __cdecl sl_test_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property&) {
		return factory.make<OrderTestComponent>();
	}
	template <typename MainLoop>
	static SafeComponentPtr __cdecl test_main_loop_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property&) {
		return factory.make<MainLoop>();
	}
	virtual ProcedurePtr find_procedure(const std::string& name) final override {
		if (name == "test_main_loop_factory")
			return reinterpret_cast<ProcedurePtr>(&factory_getter<&test_main_loop_factory<TestMainLoop>>);
		return reinterpret_cast<ProcedurePtr>(&factory_getter<&sl_test_factory>);
	}
	virtual void release() final override {
		if (order_index != test_order_load_index--)
		{
			if (fail_load_index == 0)
				fail_load_index = order_index;
		}
		delete this;
	}
};
class TestModuleLoaderOrder {
public:
	static pisk::infrastructure::ModulePtr module_loader(const std::string& basename) {
		if (basename == "system" or basename == "os")
			return nullptr;//ignore loading of system library
		return make_shared_releasable<TestModuleOrder>();
	}
};
Describe(order_load_unload_modules) {
	std::unique_ptr<TestApplicationEx<TestModuleLoaderOrder>> app;
	void SetUp() {
		test_order_load_index = 0;
		app = std::make_unique<TestApplicationEx<TestModuleLoaderOrder>>();
		Assert::That(test_order_load_index, Is().EqualTo(0U));
	}
	void TearDown() {
		if (fail_load_index)
			Assert::Failure("Delayerd fail: unexpected unload modules order");
		Assert::That(test_order_load_index, Is().EqualTo(0U));
	}
	When(no_loaded_modules) {
		Then(no_unloaded_modules) {
			Root().app->on_configure += []() {
				Assert::That(test_order_load_index, Is().EqualTo(0U));
			};
			AssertThrows(pisk::infrastructure::NullPointerException,
				Root().app->run(pisk::tools::components::DescriptionsList{}));
		}
	};
	When(one_module_loaded) {
		Then(one_module_unloaded) {
			Root().app->on_configure += []() {
				Assert::That(test_order_load_index, Is().EqualTo(1U));
			};
			Root().app->run(pisk::tools::components::DescriptionsList{
				{"service", pisk::tools::MainLoop::uid, "", "test_main_loop_factory"},
			});
		}
	};
	When(two_modules_loaded) {
		Then(two_modules_unloaded) {
			Root().app->on_configure += []() {
				Assert::That(test_order_load_index, Is().EqualTo(2U));
			};
			Root().app->run(pisk::tools::components::DescriptionsList{
				{"service", pisk::tools::MainLoop::uid, "", "test_main_loop_factory"},
				{"tst", SLTestComponent::uid, "test_sl", "sl_test_factory"},
			});
		}
	};
};

