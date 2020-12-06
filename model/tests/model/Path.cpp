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

#include <pisk/model/ReflectedItem.h>
#include <pisk/model/ReflectedObject.h>

#include <limits>

using namespace igloo;
using namespace pisk::utils;
using namespace pisk::model;

Describe(model_path) {
	Context(add_path) {
		When(add_empty_to_empty) {
			PathId path;
			void SetUp() {
				path = path.add("");
			}
			Then(result_is_empty) {
				Assert::That(path.empty(), Is().EqualTo(true));
				Assert::That(path.c_str(), Is().EqualTo(""));
			}
		};
		When(add_slash) {
			PathId path;
			void SetUp() {
				path = path.add("/");
			}
			Then(result_is_empty) {
				Assert::That(path.empty(), Is().EqualTo(true));
				Assert::That(path.c_str(), Is().EqualTo(""));
			}
		};
		When(add_slash_slash) {
			PathId path;
			void SetUp() {
				path = path.add("//");
			}
			Then(result_is_empty) {
				Assert::That(path.empty(), Is().EqualTo(true));
				Assert::That(path.c_str(), Is().EqualTo(""));
			}
		};
		When(add_backslash) {
			PathId path;
			void SetUp() {
				path = path.add("\\");
			}
			Then(result_is_empty) {
				Assert::That(path.empty(), Is().EqualTo(true));
				Assert::That(path.c_str(), Is().EqualTo(""));
			}
		};
		When(add_empty_to_parent) {
			PathId path;
			void SetUp() {
				path = path.add("parent");
				path = path.add("");
			}
			Then(result_is_parent) {
				Assert::That(path.empty(), Is().EqualTo(false));
				Assert::That(path.c_str(), Is().EqualTo("parent"));
			}
		};
		When(add_child_to_empty) {
			PathId path;
			void SetUp() {
				path = path.add("");
				path = path.add("child");
			}
			Then(result_is_child) {
				Assert::That(path.empty(), Is().EqualTo(false));
				Assert::That(path.c_str(), Is().EqualTo("child"));
			}
		};
		When(add_child_to_parent) {
			PathId path;
			void SetUp() {
				path = path.add("parent");
				path = path.add("child");
			}
			Then(result_is_child) {
				Assert::That(path.empty(), Is().EqualTo(false));
				Assert::That(path.c_str(), Is().EqualTo("parent/child"));
			}
		};
		When(add_slash_child_slash_to_slash_parent_slash) {
			PathId path;
			void SetUp() {
				path = path.add("/parent/");
				path = path.add("/child/");
			}
			Then(result_is_child) {
				Assert::That(path.c_str(), Is().EqualTo("parent/child"));
			}
		};
		When(add_slash_slash_child_slash_slash_to_slash_slash_parent_slash_slash) {
			PathId path;
			void SetUp() {
				path = path.add("\\parent\\");
				path = path.add("\\child\\");
			}
			Then(result_is_child) {
				Assert::That(path.c_str(), Is().EqualTo("parent/child"));
			}
		};
		When(add_child_to_family) {
			PathId path;
			void SetUp() {
				path = PathId::from_string("grandpa/parent");
				path = path.add("child");
			}
			Then(result_is_child) {
				Assert::That(path.c_str(), Is().EqualTo("grandpa/parent/child"));
			}
		};
		When(add_family_to_grandpa) {
			PathId path;
			void SetUp() {
				path = PathId::from_string("grandma");
				path = path.add("parent/child");
			}
			Then(result_is_child) {
				Assert::That(path.c_str(), Is().EqualTo("grandma/parent/child"));
			}
		};
		When(add_family_to_family) {
			PathId path;
			void SetUp() {
				path = PathId::from_string("grandma\\parent");
				path = path.add("child\\grandson");
			}
			Then(result_is_child) {
				Assert::That(path.c_str(), Is().EqualTo("grandma/parent/child/grandson"));
			}
		};
	};
	Context(front) {
		When(front_from_empty) {
			keystring front;
			PathId tail;
			void SetUp() {
				const auto path = PathId::from_string("");
				front = path.front();
				tail = path.tail();
			}
			Then(front_is_empty) {
				Assert::That(front.empty(), Is().EqualTo(true));
			}
			Then(tail_is_empty) {
				Assert::That(tail.empty(), Is().EqualTo(true));
			}
		};
		When(front_from_child) {
			keystring front;
			PathId tail;
			void SetUp() {
				const auto path = PathId::from_string("child");
				front = path.front();
				tail = path.tail();
			}
			Then(front_is_child) {
				Assert::That(front.c_str(), Is().EqualTo("child"));
			}
			Then(tail_is_empty) {
				Assert::That(tail.empty(), Is().EqualTo(true));
			}
		};
		When(front_from_parent_slash_child) {
			keystring front;
			PathId tail;
			void SetUp() {
				const auto path = PathId::from_string("parent/child");
				front = path.front();
				tail = path.tail();
			}
			Then(front_is_parent) {
				Assert::That(front.c_str(), Is().EqualTo("parent"));
			}
			Then(tail_is_child) {
				Assert::That(tail.c_str(), Is().EqualTo("child"));
			}
		};
		When(front_from_parent_slash_child_slash_grandson) {
			keystring front;
			PathId tail;
			void SetUp() {
				const auto path = PathId::from_string("parent/child/grandson");
				front = path.front();
				tail = path.tail();
			}
			Then(front_is_parent) {
				Assert::That(front.c_str(), Is().EqualTo("parent"));
			}
			Then(tail_is_child_grandson) {
				Assert::That(tail.c_str(), Is().EqualTo("child/grandson"));
			}
		};
	};
	Context(back) {
		When(back_from_empty) {
			keystring back;
			PathId parent;
			void SetUp() {
				const auto path = PathId::from_string("");
				back = path.back();
				parent = path.parent();
			}
			Then(back_is_empty) {
				Assert::That(back.empty(), Is().EqualTo(true));
			}
			Then(paren_empty) {
				Assert::That(parent.empty(), Is().EqualTo(true));
			}
		};
		When(back_from_child) {
			keystring back;
			PathId parent;
			void SetUp() {
				const auto path = PathId::from_string("child");
				back = path.back();
				parent = path.parent();
			}
			Then(back_is_child) {
				Assert::That(back.c_str(), Is().EqualTo("child"));
			}
			Then(parent_is_empty) {
				Assert::That(parent.empty(), Is().EqualTo(true));
			}
		};
		When(back_from_parent_slash_child) {
			keystring back;
			PathId parent;
			void SetUp() {
				const auto path = PathId::from_string("parent/child");
				back = path.back();
				parent = path.parent();
			}
			Then(back_is_child) {
				Assert::That(back.c_str(), Is().EqualTo("child"));
			}
			Then(parent_is_parent) {
				Assert::That(parent.c_str(), Is().EqualTo("parent"));
			}
		};
		When(back_from_parent_slash_child_slash_grandson) {
			keystring back;
			PathId parent;
			void SetUp() {
				const auto path = PathId::from_string("parent/child/grandson");
				back = path.back();
				parent = path.parent();
			}
			Then(back_is_grandson) {
				Assert::That(back.c_str(), Is().EqualTo("grandson"));
			}
			Then(parent_is_parent_child) {
				Assert::That(parent.c_str(), Is().EqualTo("parent/child"));
			}
		};
	};
};

