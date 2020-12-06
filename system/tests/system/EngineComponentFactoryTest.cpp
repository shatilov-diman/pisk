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

#include "TestEngineStrategy.h"

using namespace igloo;
using namespace pisk;

pisk::system::PatchPortalPtr CreatePatchPortal();


class TestComponentInstanceFactory :
	public tools::InstanceFactory
{
	infrastructure::ModulePtr module;

	virtual tools::SafeComponentPtr safe_instance(const std::shared_ptr<core::Component>& instance) const final override
	{
		return {module, instance};
	}
};


Describe(EngineFactoryTest) {
	TestComponentInstanceFactory instance_maker;
	std::unique_ptr<pisk::system::impl::EngineComponentFactory> factory;

	void SetUp() {
		factory = std::make_unique<pisk::system::impl::EngineComponentFactory>();
	}

	When(pass_nullptr_to_make_engine) {
		Then(exception_throws) {
			AssertThrows(
				pisk::infrastructure::NullPointerException,
				Root().factory->make_engine(Root().instance_maker, nullptr)
			);
		}
	};
	When(pass_valid_ptr_to_make_engine) {
		Then(no_exception) {
			auto engine = Root().factory->make_engine(Root().instance_maker, [](pisk::system::PatchRecipient&) {
				return std::make_unique<EngineStrateyTestBase>();
			});
			Root().factory->start();
			Root().factory->stop();
		}
	};
	When(engine_destroied) {
		Then(strategy_destroied) {
			auto engine = Root().factory->make_engine(Root().instance_maker, [](pisk::system::PatchRecipient&) {
				return std::make_unique<EngineStrateyTestBase>();
			});
			Root().factory->start();
			Root().factory->stop();

			Assert::That(static_cast<bool>(EngineStrateyTestBase::destroied), Is().EqualTo(false));
			engine.reset();
			Assert::That(static_cast<bool>(EngineStrateyTestBase::destroied), Is().EqualTo(true));
		}
	};
};

