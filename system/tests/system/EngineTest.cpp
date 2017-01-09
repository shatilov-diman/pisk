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
using namespace pisk::system::impl;


Describe(EngineTest) {
	When(pass_nullptr_task_to_engine) {
		Then(exception_throws) {
			AssertThrows(
				pisk::infrastructure::NullPointerException,
				std::make_shared<Engine>(nullptr)
			);
		}
	};
	When(pass_valid_ptr_to_make_engine) {
		std::shared_ptr<Engine> engine;

		void SetUp() {
			auto&& gate = std::make_unique<TestPatchGate>();
			auto&& task = make_engine_task<EngineStrateyTestBase>(std::move(gate));
			engine = std::make_shared<Engine>(std::move(task));
		}

		Then(strategy_thread_id_is_diff_from_current) {
			Assert::That(EngineStrateyTestBase::thread_id, Is().Not().EqualTo(std::this_thread::get_id()));
		}
		When(engine_destroied) {
			void SetUp() {
				Assert::That(static_cast<bool>(EngineStrateyTestBase::destroied), Is().EqualTo(false));
				Parent().engine.reset();
			}
			Then(strategy_destroied) {
				Assert::That(static_cast<bool>(EngineStrateyTestBase::destroied), Is().EqualTo(true));
			}
			/*TODO:
	 			* check thread stopes after engine destroied
			Then(thread_stops) {
				Assert::That(static_cast<bool>(EngineStrateyTestBase::destroied), Is().EqualTo(true));
			}*/
		};
	};
};

