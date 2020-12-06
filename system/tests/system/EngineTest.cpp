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

namespace pisk {
namespace system {
	EngineSynchronizerPtr make_engine_synchronizer();
}//namespace system
}//namespace pisk

using namespace igloo;
using namespace pisk;
using namespace pisk::system;

Describe(EngineTest) {
	When(pass_nullptr_task_to_engine) {
		Then(exception_throws) {
			AssertThrows(
				pisk::infrastructure::NullPointerException,
				std::make_shared<impl::Engine>(nullptr)
			);
		}
	};
	When(pass_valid_ptr_to_make_engine) {
		EngineSynchronizerPtr engine_synchronizer;
		std::shared_ptr<impl::Engine> engine;

		void SetUp() {
			engine_synchronizer = make_engine_synchronizer();

			auto&& gate = std::make_unique<TestPatchGate>();
			auto&& task = make_engine_task<EngineStrateyTestBase>(std::move(gate), engine_synchronizer->make_slave());
			engine = std::make_shared<impl::Engine>(std::move(task));

			engine_synchronizer->initialize_signal();
			engine_synchronizer->run_loop_signal();
			engine_synchronizer->stop_all();
			engine_synchronizer->deinitialize_signal();
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
	 			* check thread will stopped after engine has had destroied
			Then(thread_stops) {
				Assert::That(static_cast<bool>(EngineStrateyTestBase::destroied), Is().EqualTo(true));
			}*/
		};
	};
};

