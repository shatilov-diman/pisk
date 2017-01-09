// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
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
#include <pisk/utils/keystring.h>
#include <pisk/utils/algorithm_utils.h>
#include <pisk/utils/json_utils.h>
#include <pisk/tools/ComponentsLoader.h>
#include <pisk/tools/EventsContainer.h>
#include <pisk/tools/OsAppInstance.h>

using namespace igloo;
using namespace pisk::tools;
using namespace pisk::tools::components;

class TestServiceRegistry :
	public pisk::tools::ServiceRegistry
{
	std::map<UID, SafeComponentPtr> sl;

	virtual SafeComponentPtr find(const UID& uid) const final override
	{
		auto found = sl.find(uid);
		if (found == sl.end())
			return {};
		return found->second;
	}
public:
	void clear()
	{
		sl.clear();
	}
	void replace(const UID& key, const SafeComponentPtr& component)
	{
		sl[key] = component;
	}
};
class TestComponentInstanceFactory :
	public pisk::tools::InstanceFactory
{
	virtual SafeComponentPtr safe_instance(const std::shared_ptr<pisk::core::Component>& instance) const final override
	{
		return {nullptr, instance};
	}
};

static const std::string make_jcomp(const std::string& type, const std::string& name, const std::string& module, const std::string& factory, const std::string& extend)
{
	return pisk::utils::string::format(std::string(
		"{"
		"%s"
		"\"type\":\"%s\","
		"\"component\":\"%s\","
		"\"module\":\"%s\","
		"\"factory\":\"%s\""
		"}"),
		extend.c_str(), type.c_str(), name.c_str(), module.c_str(), factory.c_str());
}

using component_tuple = std::tuple<std::string, std::string, std::string, std::string>;
using component_tuple_with_deps = std::tuple<std::string, std::string, std::string, std::string, std::vector<std::string>>;
using component_tuple_with_config = std::tuple<std::string, std::string, std::string, std::string, pisk::utils::property>;

static const std::string make_jcomponents(const std::vector<component_tuple>& components)
{
	std::vector<std::string> parts;
	std::transform(components.begin(), components.end(), std::back_inserter(parts), [](const component_tuple& comp) {
		return make_jcomp(std::get<0>(comp), std::get<1>(comp), std::get<2>(comp), std::get<3>(comp), {});
	});
	return '[' + pisk::utils::algorithm::join(parts, std::string(",")) + ']';
}

static const std::string make_jconfig(const pisk::utils::property& config)
{
	return "\"config\":" + pisk::utils::json::to_string(config) + ",";
}

static const std::string make_jdependecies(const std::vector<std::string>& dependecies)
{
	return "\"dependencies\":[" + pisk::utils::algorithm::join(dependecies, std::string(",")) + "],";
}

static const std::string make_jcomponents_with_deps(const std::vector<component_tuple_with_deps>& components)
{
	std::vector<std::string> parts;
	std::transform(components.begin(), components.end(), std::back_inserter(parts), [](const component_tuple_with_deps& comp) {
		const std::string& deps = make_jdependecies(std::get<4>(comp));
		return make_jcomp(std::get<0>(comp), std::get<1>(comp), std::get<2>(comp), std::get<3>(comp), deps);
	});
	return '[' + pisk::utils::algorithm::join(parts, std::string(",")) + ']';
}
static const std::string make_jcomponents_with_config(const std::vector<component_tuple_with_config>& components)
{
	std::vector<std::string> parts;
	std::transform(components.begin(), components.end(), std::back_inserter(parts), [](const component_tuple_with_config& comp) {
		const std::string& config = make_jconfig(std::get<4>(comp));
		return make_jcomp(std::get<0>(comp), std::get<1>(comp), std::get<2>(comp), std::get<3>(comp), config);
	});
	return '[' + pisk::utils::algorithm::join(parts, std::string(",")) + ']';
}

class BaseTestDataStream : public pisk::infrastructure::DataStream {
	virtual std::size_t tell() const { throw pisk::infrastructure::LogicErrorException(); }
	virtual std::size_t seek(const long, const Whence) { throw pisk::infrastructure::LogicErrorException(); }
	virtual std::size_t read(const std::size_t, pisk::infrastructure::DataBuffer&) { throw pisk::infrastructure::LogicErrorException(); }
};

class ComponentsJsonDataStream : public BaseTestDataStream {
	virtual pisk::infrastructure::DataBuffer readall() const override final {
		static const std::string& data = make_jcomponents({ std::make_tuple("srv", "loc", "test", "f_loc") });
		return pisk::infrastructure::DataBuffer(data.begin(), data.end());
	}
};

class SameNameComponentsJsonDataStream : public BaseTestDataStream {
	virtual pisk::infrastructure::DataBuffer readall() const override final {
		static const std::string& data = make_jcomponents({ std::make_tuple("dev", "loc", "test", "f_loc"), std::make_tuple("eng", "loc", "test2", "f_loc2") });
		return pisk::infrastructure::DataBuffer(data.begin(), data.end());
	}
};

class TwoComponentsJsonDataStream : public BaseTestDataStream {
	virtual pisk::infrastructure::DataBuffer readall() const override final {
		static const std::string& data = make_jcomponents({ std::make_tuple("srv", "res", "module", "f_srv"), std::make_tuple("eng", "loc", "module", "f_loc") });
		return pisk::infrastructure::DataBuffer(data.begin(), data.end());
	}
};

class TwoComponentsJsonDataStreamWithDependecies : public BaseTestDataStream {
	virtual pisk::infrastructure::DataBuffer readall() const override final {
		static const std::string& data = make_jcomponents_with_deps({ std::make_tuple("srv1", "res1", "module1", "f_srv", std::vector<std::string>{}), std::make_tuple("eng2", "loc2", "module2", "f_loc", std::vector<std::string>{"123", "\"second\""}) });
		return pisk::infrastructure::DataBuffer(data.begin(), data.end());
	}
};

class TwoComponentsJsonDataStreamWithConfig : public BaseTestDataStream {
	virtual pisk::infrastructure::DataBuffer readall() const override final {
		static const pisk::utils::property cfg1 = "config.cfg";
		static const pisk::utils::property cfg2 = pisk::utils::property::dictionary{{"config_data", 15}};
		static const std::string& data = make_jcomponents_with_config({ std::make_tuple("srv1", "res1", "module1", "f_srv", cfg1), std::make_tuple("eng2", "loc2", "module2", "f_loc", cfg2) });
		return pisk::infrastructure::DataBuffer(data.begin(), data.end());
	}
};

class RawStringComponentStream : public BaseTestDataStream {
	const std::string _data;
	virtual pisk::infrastructure::DataBuffer readall() const override final {
		return pisk::infrastructure::DataBuffer(_data.begin(), _data.end());
	}
public:
	explicit RawStringComponentStream(const std::string& data) :
		_data(data)
	{}
};


Describe(components_ParserTest) {
	When(given_non_json) {
		Then(list_is_empty) {
			const auto data = std::make_unique<RawStringComponentStream>("][");
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList{}));
		}
	};
	When(given_int_json) {
		Then(list_is_empty) {
			const auto data = std::make_unique<RawStringComponentStream>("13");
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList {}));
                }
	};
	When(given_empty_dict_json) {
		Then(list_is_empty) {
			const auto data = std::make_unique<RawStringComponentStream>("{}");
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList {}));
                }
	};
	When(given_wrong_types) {
		Then(list_is_empty) {
			const auto data = std::make_unique<RawStringComponentStream>(
"[{ \"type\" : 146, \"component\" : 146, \"module\" : 146, \"factory\" : 146, }]"
			);
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList {}));
		}
	};
	When(given_wrong_dependecy_types) {
		Then(list_is_test_without_dependecies) {
			const auto data = std::make_unique<RawStringComponentStream>(
"[{ \"type\" : \"test1\", \"component\" : \"test1\", \"module\" : \"test1\", \"factory\" : \"test1\", \"dependencies\":\"xxx\" }, \
  { \"type\" : \"test2\", \"component\" : \"test2\", \"module\" : \"test2\", \"factory\" : \"test2\", \"dependencies\":{} }]"
			);
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList {{"test1", "test1", "test1", "test1"}, {"test2", "test2", "test2", "test2"}}));
		}
	};
	When(given_without_dependecy) {
		Then(list_is_test_without_dependecies) {
			const auto data = std::make_unique<RawStringComponentStream>(
"[{ \"type\" : \"test\", \"component\" : \"test\", \"module\" : \"test\", \"factory\" : \"test\" }]"
			);
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList {{"test", "test", "test", "test"}}));
		}
	};
	When(given_empty_dependecy) {
		Then(list_is_test_without_dependecies) {
			const auto data = std::make_unique<RawStringComponentStream>(
"[{ \"type\" : \"test\", \"component\" : \"test\", \"module\" : \"test\", \"factory\" : \"test\", \"dependencies\":[] }]"
			);
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList {{"test", "test", "test", "test"}}));
		}
	};
	When(given_dependecies) {
		Then(list_is_test_without_dependecies) {
			const auto data = std::make_unique<RawStringComponentStream>(
"[{ \"type\" : \"test\", \"component\" : \"test\", \"module\" : \"test\", \"factory\" : \"test\", \"dependencies\":[ \
\"first\", \"second\" \
] }]"
			);
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList {{"test", "test", "test", "test", {}, {"first", "second"}}}));
		}
	};
	When(given_dependecies_with_wrong_typr) {
		Then(list_is_test_without_dependecies) {
			const auto data = std::make_unique<RawStringComponentStream>(
"[{ \"type\" : \"test\", \"component\" : \"test\", \"module\" : \"test\", \"factory\" : \"test\", \"dependencies\":[ \
\"first\", 123 \
] }]"
			);
			const auto& desc = Parser::parse(*data);
			Assert::That(desc, EqualsContainer(DescriptionsList {{"test", "test", "test", "test", {}, {"first"}}}));
		}
	};
};

class OsAppInstanceImpl :
	public OsAppInstance
{
	virtual void release() final override
	{}
};


Describe(components_LoaderTest) {
	std::function<SafeComponentPtr (components::ComponentFactoryFn factory, pisk::infrastructure::ModulePtr module, const pisk::utils::property& config)> component_loader;
	std::function<void (const std::string& name, SafeComponentPtr component)> components_storage;

	TestServiceRegistry temp_sl;
	EventsContainer events;

	std::vector<SafeComponentPtr> components;

	void SetUp() {
		component_loader = [this](components::ComponentFactoryFn factory, pisk::infrastructure::ModulePtr module, const pisk::utils::property& config) {
			TestComponentInstanceFactory instance_maker;
			return factory(temp_sl, instance_maker, config);
		};
		components_storage = [this](const std::string& name, SafeComponentPtr component) {
			components.push_back(component);
			temp_sl.replace(pisk::utils::keystring(name), component);
		};
	}
	When(modules_loader_returns_nullptr) {
		static pisk::infrastructure::ModulePtr load_null_module(const std::string&) {
			return nullptr;
		}

		Then(store_component_doesn_not_call) {
			const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
			const auto& desc = Parser::parse(*data);
			Loader::load(desc, &load_null_module, Root().component_loader, Root().components_storage);
			Assert::That(Root().components, Is().Empty());
		}
	};
	When(module_returns_null) {
		struct TestModuleNullProcedure : public pisk::infrastructure::Module {
			virtual ProcedurePtr find_procedure(const std::string&) final override {
				return nullptr;
			}
			virtual void release() final override {
				delete this;
			}
			static pisk::infrastructure::ModulePtr load(const std::string&) {
				return make_unique_releasable<TestModuleNullProcedure>();
			}
		};

		Then(store_component_doesn_not_call) {
			const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
			const auto& desc = Parser::parse(*data);
			Loader::load(desc, &TestModuleNullProcedure::load, Root().component_loader, Root().components_storage);
			Assert::That(Root().components, Is().Empty());
		}
	};
	When(factory_getter_returns_null) {
		static ComponentFactory __cdecl test_null_factory_getter() {
			return nullptr;
		}
		struct TestModule : public pisk::infrastructure::Module {
			virtual ProcedurePtr find_procedure(const std::string&) final override {
				return reinterpret_cast<ProcedurePtr>(&test_null_factory_getter);
			}
			virtual void release() final override {
				delete this;
			}
			static pisk::infrastructure::ModulePtr load(const std::string&) {
				return make_unique_releasable<TestModule>();
			}
		};

		Then(ComponenetsLoader_returns_empty_container) {
			const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
			const auto& desc = Parser::parse(*data);
			Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage);
			Assert::That(Root().components, Is().Empty());
		}
	};
	When(factory_returns_null) {
		static SafeComponentPtr __cdecl test_null_factory(const ServiceRegistry&, const InstanceFactory&, const pisk::utils::property&) {
			return {};
		}
		static ComponentFactory __cdecl test_null_factory_getter() {
			return &test_null_factory;
		}
		struct TestModule : public pisk::infrastructure::Module {
			virtual ProcedurePtr find_procedure(const std::string&) final override {
				return reinterpret_cast<ProcedurePtr>(&test_null_factory_getter);
			}
			virtual void release() final override {
				delete this;
			}
			static pisk::infrastructure::ModulePtr load(const std::string&) {
				return make_unique_releasable<TestModule>();
			}
		};

		Then(ComponenetsLoader_returns_empty_container) {
			const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
			const auto& desc = Parser::parse(*data);
			Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage);
			Assert::That(Root().components, Is().Empty());
		}
	};
	When(factory_getter_throw_exception) {
		static ComponentFactory __cdecl test_null_factory_getter() {
			throw std::domain_error("Test exception");
		}
		struct TestModule : public pisk::infrastructure::Module {
			virtual ProcedurePtr find_procedure(const std::string&) final override {
				return reinterpret_cast<ProcedurePtr>(&test_null_factory_getter);
			}
			virtual void release() final override {
				delete this;
			}
			static pisk::infrastructure::ModulePtr load(const std::string&) {
				return make_unique_releasable<TestModule>();
			}
		};

		Then(ComponenetsLoader_returns_empty_container) {
			const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
			const auto& desc = Parser::parse(*data);
			AssertThrows(std::domain_error, Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage));
			Assert::That(Root().components, Is().Empty());
		}
	};
	When(factory_throw_exception) {
		static SafeComponentPtr __cdecl test_null_factory(const ServiceRegistry&, const InstanceFactory&, const pisk::utils::property&) {
			throw std::domain_error("Test exception");
		}
		static ComponentFactory __cdecl test_null_factory_getter() {
			return &test_null_factory;
		}
		struct TestModule : public pisk::infrastructure::Module {
			virtual ProcedurePtr find_procedure(const std::string&) final override {
				return reinterpret_cast<ProcedurePtr>(&test_null_factory_getter);
			}
			virtual void release() final override {
				delete this;
			}
			static pisk::infrastructure::ModulePtr load(const std::string&) {
				return make_unique_releasable<TestModule>();
			}
		};

		Then(ComponenetsLoader_returns_empty_container) {
			const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
			const auto& desc = Parser::parse(*data);
			AssertThrows(std::domain_error, Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage));
			Assert::That(Root().components, Is().Empty());
		}
	};
	When(component_loaded) {
		struct TestComponent : public pisk::core::Component {
			pisk::utils::property config;

			explicit TestComponent(const pisk::utils::property& config):
				config(config)
			{}
			virtual void release() final override { delete this; }
		};
		static SafeComponentPtr __cdecl test_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property& config) {
			return factory.make<TestComponent>(config);
		}
		static ComponentFactory __cdecl test_factory_getter() {
			return &test_factory;
		}
		struct TestModule: public pisk::infrastructure::Module {
			virtual ProcedurePtr find_procedure(const std::string&) final override {
				return reinterpret_cast<ProcedurePtr>(&test_factory_getter);
			}
			virtual void release() final override {
				delete this;
			}
			static pisk::infrastructure::ModulePtr load(const std::string&) {
				++get_max_counter();
				return make_unique_releasable<TestModule>();
			}
			static int& get_max_counter()
			{
				static int counter;
				return counter;
			}
		};

		void SetUp()
		{
			TestModule::get_max_counter() = 0;
		}

		Then(ComponenetsLoader_returns_container_with_one_component) {
			const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
			const auto& desc = Parser::parse(*data);
			Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage);
			Assert::That(Root().components, HasLength(1));
		}
		When(module_loader_is_null) {
			Then(load_only_first) {
				const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
				const auto& desc = Parser::parse(*data);
				AssertThrows(pisk::infrastructure::NullPointerException,
					Loader::load(desc, nullptr, Root().component_loader, Root().components_storage)
				);
				Assert::That(Root().components, Is().Empty());
			}
		};
		When(component_loader_is_null) {
			Then(load_only_first) {
				const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
				const auto& desc = Parser::parse(*data);
				AssertThrows(pisk::infrastructure::NullPointerException,
					Loader::load(desc, &TestModule::load, nullptr, Root().components_storage)
				);
				Assert::That(Root().components, Is().Empty());
			}
		};
		When(component_store_is_null) {
			Then(load_only_first) {
				const pisk::infrastructure::DataStreamPtr data = std::make_unique<ComponentsJsonDataStream>();
				const auto& desc = Parser::parse(*data);
				AssertThrows(pisk::infrastructure::NullPointerException,
					Loader::load(desc, &TestModule::load, Root().component_loader, nullptr)
				);
				Assert::That(Root().components, Is().Empty());
			}
		};
		When(components_contains_two_component_with_same_name) {
			Then(load_only_first) {
				const pisk::infrastructure::DataStreamPtr data = std::make_unique<SameNameComponentsJsonDataStream>();
				const auto& desc = Parser::parse(*data);
				Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage);
				Assert::That(Root().components, HasLength(1));
			}
		};
		When(components_contains_two_component_with_dependency) {
			Then(load_both) {
				const pisk::infrastructure::DataStreamPtr data = std::make_unique<TwoComponentsJsonDataStreamWithDependecies>();
				const auto& desc = Parser::parse(*data);
				Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage);
				Assert::That(Root().components, HasLength(2));
				Assert::That(TestModule::get_max_counter(), Is().EqualTo(3));
			}
		};
		When(components_contains_two_component_with_config) {
			Then(load_both) {
				const pisk::infrastructure::DataStreamPtr data = std::make_unique<TwoComponentsJsonDataStreamWithConfig>();
				const auto& desc = Parser::parse(*data);
				Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage);
				Assert::That(Root().components, HasLength(2));
				Assert::That(Root().components[0].cast<TestComponent>()->config.as_string(), Is().EqualTo("config.cfg"));
				Assert::That(Root().components[1].cast<TestComponent>()->config["config_data"].as_double(), Is().EqualTo(15.));
			}
		};
	};
	When(factory_require_another_component) {
		struct TestComponent0 : public pisk::core::Component {
			virtual void release() final override { delete this; }
		};
		struct TestComponent1 : public pisk::core::Component {
			pisk::tools::InterfacePtr<TestComponent0> another;
			explicit TestComponent1(const pisk::tools::InterfacePtr<TestComponent0>& another) :
				another(another)
			{}
			virtual void release() final override { delete this; }
		};
		static SafeComponentPtr __cdecl srv_test_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property&) {
			return factory.make<TestComponent0>();
		}
		static ComponentFactory __cdecl srv_test_factory_getter() {
			return &srv_test_factory;
		}
		static SafeComponentPtr __cdecl test_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&) {
			auto x = temp_sl.get<TestComponent0>("res");
			Assert::That(x, Is().Not().EqualTo(nullptr));
			return factory.make<TestComponent1>(x);
		}
		static ComponentFactory __cdecl test_factory_getter() {
			return &test_factory;
		}
		struct TestModule : public pisk::infrastructure::Module {
			virtual ProcedurePtr find_procedure(const std::string& name) final override {
				if (name == "f_srv")
					return reinterpret_cast<ProcedurePtr>(&srv_test_factory_getter);
				return reinterpret_cast<ProcedurePtr>(&test_factory_getter);
			}
			virtual void release() final override {
				delete this;
			}
			static pisk::infrastructure::ModulePtr load(const std::string&) {
				return make_unique_releasable<TestModule>();
			}
		};

		Then(ComponenetsLoader_returns_container_with_one_component) {
			const pisk::infrastructure::DataStreamPtr data = std::make_unique<TwoComponentsJsonDataStream>();
			const auto& desc = Parser::parse(*data);
			Loader::load(desc, &TestModule::load, Root().component_loader, Root().components_storage);
			Assert::That(Root().components, HasLength(2));
		}
	};
};

