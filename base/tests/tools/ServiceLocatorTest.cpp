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
#include <pisk/utils/algorithm_utils.h>

#include "../../sources/tools/ServiceLocator.h"

#include <typeindex>
#include <atomic>

using namespace igloo;
using namespace pisk::tools;

namespace {

static std::atomic_int components_count;

struct BaseComponent
{
	BaseComponent()
	{
		++components_count;
	}
	virtual ~BaseComponent()
	{
		--components_count;
	}
};

struct TestComponent1
{
	static std::size_t generate_uid() { return 7; }
	virtual void doit() = 0;
};
struct TestComponent2
{
	static std::size_t generate_uid() { return 775; }
	virtual void doit2() = 0;
};

struct TestComponent1Impl :
	BaseComponent,
	TestComponent1
{
	virtual void doit() {}
};
struct TestComponent2Impl :
	BaseComponent,
	TestComponent2
{
	virtual void doit2() {}
};
struct TestComponent12Impl :
	BaseComponent,
	TestComponent1,
	TestComponent2
{
	virtual void doit() {}

	virtual void doit2() {}
};


static std::atomic_int holders_count;

struct ComponentHolder
{
	ComponentHolder()
	{
		++holders_count;
	}
	ComponentHolder(const ComponentHolder& holder) :
		component(holder.component),
		internal_data(holder.internal_data)
	{
		 ++holders_count;
	}
	ComponentHolder(std::shared_ptr<BaseComponent> component, int data) :
		component(component),
		internal_data(data)
	{
		 ++holders_count;
	}
	virtual ~ComponentHolder()
	{
		--holders_count;
	}

	std::shared_ptr<BaseComponent> component;
	int internal_data = 42;
};

struct ComponentToInterfaceExtractor
{
	template <typename Interface>
	using InterfacePtr = std::shared_ptr<Interface>;

	using UID = std::size_t;

	template <typename Interface>
	static InterfacePtr<Interface> extract(const ComponentHolder& holder)
	{
		return std::dynamic_pointer_cast<Interface>(holder.component);
	}
};

class TestServiceLocator : public ServiceLocator<ComponentHolder, ComponentToInterfaceExtractor>
{
	using base = ServiceLocator<ComponentHolder, ComponentToInterfaceExtractor>;
public:
	template <typename Interface>
	void add(const ComponentHolder& holder) {
		base::add(Interface::generate_uid(), holder);
	}
	template <typename Interface>
	void replace(const ComponentHolder& holder) {
		base::replace(Interface::generate_uid(), holder);
	}
	template <typename Interface>
	void remove() {
		base::remove(Interface::generate_uid());
	}
	template <typename Interface>
	base::template InterfacePtr<Interface> get() {
		return base::get<Interface>(Interface::generate_uid());
	}
};

Describe(ServiceLocatorTest) {
	TestServiceLocator sl;

	void SetUp() {
		components_count = 0;
		holders_count = 0;
	}
	void TearDown() {
		sl.clear();
		check_counter(0);
	}
	void check_components_counter(const int expected_count)
	{
		Assert::That(static_cast<int>(components_count), Is().EqualTo(expected_count));
	}
	void check_holders_counter(const int expected_count)
	{
		Assert::That(static_cast<int>(holders_count), Is().EqualTo(expected_count));
	}
	void check_counter(const int expected_count)
	{
		check_components_counter(expected_count);
		check_holders_counter(expected_count);
	}

	When(itnerface_does_not_add) {
		Then(it_cannot_be_found) {
			auto instance = Root().sl.get<TestComponent1>();
			Assert::That(instance, Is().EqualTo(nullptr));
		}
	};
	When(add_interface) {
		void SetUp() {
			ComponentHolder holder {std::make_shared<TestComponent1Impl>(), 13};
			Root().sl.add<TestComponent1>(holder);
		}
		Then(it_can_be_found) {
			auto instance = Root().sl.get<TestComponent1>();
			Assert::That(instance, Is().Not().EqualTo(nullptr));
			Root().check_counter(1);
		}
		Then(another_cannot_be_found) {
			auto instance = Root().sl.get<TestComponent2>();
			Assert::That(instance, Is().EqualTo(nullptr));
			Root().check_counter(1);
		}
		When(remove_interface) {
			void SetUp() {
				Root().sl.remove<TestComponent1>();
				Root().check_counter(0);
			}
			Then(it_can_be_found_again) {
				auto instance = Root().sl.get<TestComponent1>();
				Assert::That(instance, Is().EqualTo(nullptr));
				Root().check_counter(0);
			}
			Then(another_still_cannot_be_found) {
				auto instance = Root().sl.get<TestComponent2>();
				Assert::That(instance, Is().EqualTo(nullptr));
				Root().check_counter(0);
			}
		};
		When(add_another_interface) {
			void SetUp() {
				ComponentHolder holder {std::make_shared<TestComponent2Impl>(), 13};
				Root().sl.add<TestComponent2>(holder);
			}
			Then(it_can_be_found) {
				auto instance = Root().sl.get<TestComponent1>();
				Assert::That(instance, Is().Not().EqualTo(nullptr));
				Root().check_counter(2);
			}
			Then(another_can_be_found) {
				auto instance = Root().sl.get<TestComponent2>();
				Assert::That(instance, Is().Not().EqualTo(nullptr));
				Root().check_counter(2);
			}
		};
		When(add_another_interface_with_same_id) {
			Then(exception_should_thrown) {
				ComponentHolder holder {std::make_shared<TestComponent12Impl>(), 13};
				AssertThrowsEx(details::DuplicateFoundException, Root().sl.add<TestComponent1>(holder));
			}
		};
		When(replace_interface_with_another_interface) {
			void SetUp() {
				ComponentHolder holder {std::make_shared<TestComponent12Impl>(), 13};
				Root().sl.replace<TestComponent1>(holder);
				Root().sl.replace<TestComponent2>(holder);
			}
			Then(it_can_be_found) {
				auto instance = Root().sl.get<TestComponent1>();
				Assert::That(instance, Is().Not().EqualTo(nullptr));
				Assert::That(std::type_index(typeid(decltype((*instance.get())))), Is().EqualTo(std::type_index(typeid(TestComponent1))));
				Root().check_components_counter(1);
				Root().check_holders_counter(2);
			}
			Then(another_can_be_found) {
				auto instance = Root().sl.get<TestComponent2>();
				Assert::That(instance, Is().Not().EqualTo(nullptr));
				Assert::That(std::type_index(typeid(decltype((*instance.get())))), Is().EqualTo(std::type_index(typeid(TestComponent2))));
				Root().check_components_counter(1);
				Root().check_holders_counter(2);
			}
		};
	};
};

}

