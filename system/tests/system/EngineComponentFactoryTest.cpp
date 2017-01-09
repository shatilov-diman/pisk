// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module system of the project pisk.
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
		pisk::system::PatchPortalPtr&& portal = CreatePatchPortal();
		factory = std::make_unique<pisk::system::impl::EngineComponentFactory>(std::move(portal));
	}

	When(pass_nullptr_to_constructor) {
		Then(exception_throws) {
			AssertThrows(
				pisk::infrastructure::NullPointerException,
				std::make_unique<pisk::system::impl::EngineComponentFactory>(nullptr)
			);
		}
	};
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
		}
	};
	When(engine_destroied) {
		Then(strategy_destroied) {
			auto engine = Root().factory->make_engine(Root().instance_maker, [](pisk::system::PatchRecipient&) {
				return std::make_unique<EngineStrateyTestBase>();
			});
			Assert::That(static_cast<bool>(EngineStrateyTestBase::destroied), Is().EqualTo(false));
			engine.reset();
			Assert::That(static_cast<bool>(EngineStrateyTestBase::destroied), Is().EqualTo(true));
		}
	};
};

