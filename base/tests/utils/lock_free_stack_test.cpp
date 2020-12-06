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
#include <pisk/utils/lock_free_stack.h>

#include <functional>
#include <set>

using namespace igloo;
using namespace pisk::utils;

Describe(lock_free_stack_int) {
	lock_free_stack<int> ints;
	When(just_initialized) {
		Then(pop_returns_false) {
			int x = 5;
			Assert::That(Root().ints.pop(x), Is().EqualTo(false));
			Assert::That(x, Is().EqualTo(5));
		}
		When(push_7_and_pop_one) {
			void SetUp() {
				Root().ints.push(7);
			}
			Then(popped_is_7_and_no_more) {
				int x = 23;
				Assert::That(Root().ints.pop(x), Is().EqualTo(true));
				Assert::That(x, Is().EqualTo(7));
				int y = -33;
				Assert::That(Root().ints.pop(y), Is().EqualTo(false));
				Assert::That(y, Is().EqualTo(-33));
			}
		};
		When(push_17_and_42) {
			void SetUp() {
				Root().ints.push(17);
				Root().ints.push(42);
			}
			Then(popped_is_42_and_then_17) {
				int x = 9;
				Assert::That(Root().ints.pop(x), Is().EqualTo(true));
				Assert::That(x, Is().EqualTo(42));
				Assert::That(Root().ints.pop(x), Is().EqualTo(true));
				Assert::That(x, Is().EqualTo(17));
				int y = -15;
				Assert::That(Root().ints.pop(y), Is().EqualTo(false));
				Assert::That(y, Is().EqualTo(-15));
			}
		};
	};
};

