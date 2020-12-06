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

#include <pisk/utils/safequeue.h>

#include <functional>

using namespace igloo;
using namespace pisk;
using namespace pisk::utils;

Describe(test_safequeue) {
	safequeue<int> queue;

	It(empty_by_default) {
		Assert::That(Root().queue.empty(), Is().EqualTo(true));
	}
	It(size_is_0) {
		Assert::That(Root().queue.size(), Is().EqualTo(0U));
	}
	When(pop_empty) {
		int value1 = -1;
		int value2 = -2;
		Then(pop_returns_false) {
			Assert::That(Root().queue.pop(value1), Is().EqualTo(false));
		}
		Then(value_doesnot_changed) {
			Assert::That(Root().queue.pop(value2), Is().EqualTo(false));
			Assert::That(value2, Is().EqualTo(-2));
			Assert::That(Root().queue.pop(value1), Is().EqualTo(false));
			Assert::That(value1, Is().EqualTo(-1));
		}
	};
	When(push_rvalue) {
		void SetUp() {
			int rvalue = 8;
			Root().queue.push(std::move(rvalue));
		}
		Then(queue_is_not_empty) {
			Assert::That(Root().queue.empty(), Is().EqualTo(false));
		}
		Then(size_is_1) {
			Assert::That(Root().queue.size(), Is().EqualTo(1U));
		}
		When(pop_value) {
			int last_value = -1;
			void SetUp() {
				Assert::That(Root().queue.pop(last_value), Is().EqualTo(true));
			}
			Then(queue_is_empty) {
				Assert::That(Root().queue.empty(), Is().EqualTo(true));
			}
			Then(popped_value_is_7) {
				Assert::That(last_value, Is().EqualTo(8));
			}
			Then(size_is_0) {
				Assert::That(Root().queue.size(), Is().EqualTo(0U));
			}
		};
	};
	When(push_7) {
		void SetUp() {
			Root().queue.push(7);
		}
		Then(queue_is_not_empty) {
			Assert::That(Root().queue.empty(), Is().EqualTo(false));
		}
		Then(size_is_1) {
			Assert::That(Root().queue.size(), Is().EqualTo(1U));
		}
		When(pop_value) {
			int last_value = -1;
			void SetUp() {
				Assert::That(Root().queue.pop(last_value), Is().EqualTo(true));
			}
			Then(queue_is_empty) {
				Assert::That(Root().queue.empty(), Is().EqualTo(true));
			}
			Then(popped_value_is_7) {
				Assert::That(last_value, Is().EqualTo(7));
			}
			Then(size_is_0) {
				Assert::That(Root().queue.size(), Is().EqualTo(0U));
			}
		};
	};
	When(push_1_and_2) {
		void SetUp() {
			Root().queue.push(1);
			Root().queue.push(2);
		}
		Then(size_is_2) {
			Assert::That(Root().queue.size(), Is().EqualTo(2U));
		}
		Then(popped_1_and_2) {
			int v1 = -1;
			int v2 = -1;
			Assert::That(Root().queue.pop(v1), Is().EqualTo(true));
			Assert::That(Root().queue.pop(v2), Is().EqualTo(true));
			Assert::That(v1, Is().EqualTo(1));
			Assert::That(v2, Is().EqualTo(2));
		}
	};
};

