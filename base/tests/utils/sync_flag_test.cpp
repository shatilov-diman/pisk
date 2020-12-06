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
#include <pisk/infrastructure/Exception.h>

#include <pisk/utils/sync_flag.h>

#include <chrono>
#include <future>
#include <thread>

using namespace igloo;
using namespace pisk;
using namespace pisk::utils;

Describe(test_sync_flag) {
	sync::flag flag;
	It(get_is_false) {
		Assert::That(Root().flag.get(), Is().EqualTo(false));
	}
	When(set) {
		void SetUp() {
			Root().flag.set();
		}
		Then(get_is_true) {
			Assert::That(Root().flag.get(), Is().EqualTo(true));
		}
		Then(no_wait) {
			const auto begin = std::chrono::system_clock::now();
			Root().flag.wait();
			const auto end = std::chrono::system_clock::now();
			const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
			Assert::That(duration.count(), Is().LessThan(1));
		}
		Then(can_set_more) {
			Root().flag.set();
			Assert::That(Root().flag.get(), Is().EqualTo(true));
		}
	};
	When(wait_for_10ms) {
		std::chrono::milliseconds duration;
		void SetUp() {
			const auto begin = std::chrono::system_clock::now();
			Assert::That(Root().flag.wait_for(std::chrono::milliseconds(10)), Is().EqualTo(false));
			const auto end = std::chrono::system_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
		}
		Then(duration_at_least_10ms) {
			Assert::That(duration.count(), Is().GreaterThan(9));
			Assert::That(duration.count(), Is().LessThan(30));
		}
		Then(get_is_false) {
			Assert::That(Root().flag.get(), Is().EqualTo(false));
		}
	};
	When(set_async) {
		void SetUp() {
		}
		Then(no_wait) {
			const auto begin = std::chrono::system_clock::now();
			std::async(std::launch::async, [this]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				Root().flag.set();
			});
			Root().flag.wait();
			const auto end = std::chrono::system_clock::now();
			const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
			Assert::That(duration.count(), Is().GreaterThan(9));
			Assert::That(duration.count(), Is().LessThan(30));
		}
	};
	Describe(can_set_from_several_threads) {
		sync::flag flag_sync;
		It(no_wait) {
			const auto begin = std::chrono::system_clock::now();
			auto a1 = std::async(std::launch::async, [this]() {
				flag_sync.wait();
				Root().flag.set();
			});
			auto a2 = std::async(std::launch::async, [this]() {
				flag_sync.wait();
				Root().flag.set();
			});
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			flag_sync.set();
			Root().flag.wait();
			const auto end = std::chrono::system_clock::now();
			const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
			a1.wait();
			a2.wait();
			Assert::That(duration.count(), Is().GreaterThan(9));
			Assert::That(duration.count(), Is().LessThan(30));
		}
	};
};


