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

#include <pisk/model/ReflectedEvent.h>

using namespace igloo;
using namespace pisk::utils;
using namespace pisk::model;

Describe(TestReflectedEvent) {
	property orig;
	property diff;

	ReflectedEvent event{orig, diff};
	const ReflectedEvent& cevent = event;

	Spec(initial_values_are_none) {
		Assert::That(Root().event.source().is_none(), Is().EqualTo(true));
		Assert::That(Root().event.action().is_none(), Is().EqualTo(true));
		Assert::That(Root().event.type().is_none(), Is().EqualTo(true));
		Assert::That(Root().cevent.source().is_none(), Is().EqualTo(true));
		Assert::That(Root().cevent.action().is_none(), Is().EqualTo(true));
		Assert::That(Root().cevent.type().is_none(), Is().EqualTo(true));
	}
	When(set_source) {
		void SetUp() {
			Root().event.source() = "SRC";
		}
		Then(source_is_available) {
			Assert::That(Root().event.source().is_string(), Is().EqualTo(true));
			Assert::That(Root().event.source().as_string(), Is().EqualTo("SRC"));
			Assert::That(Root().cevent.source().as_string(), Is().EqualTo("SRC"));
		}
		Spec(check_diff) {
			property check;
			check["source"] = "SRC";
			Assert::That(Root().diff, Is().EqualTo(check));
		}
	};
	When(set_not_string_source) {
		void SetUp() {
			Root().event.source() = 123;
		}
		Then(source_is_available) {
			AssertThrowsEx(UnexpectedItemTypeException, Root().cevent.source());
		}
	};
	When(set_type) {
		void SetUp() {
			Root().event.type() = "TYPE";
		}
		Then(type_is_available) {
			Assert::That(Root().event.type().is_string(), Is().EqualTo(true));
			Assert::That(Root().event.type().as_string(), Is().EqualTo("TYPE"));
			Assert::That(Root().cevent.type().as_string(), Is().EqualTo("TYPE"));
		}
		Spec(check_diff) {
			property check;
			check["type"] = "TYPE";
			Assert::That(Root().diff, Is().EqualTo(check));
		}
	};
	When(set_not_string_type) {
		void SetUp() {
			Root().event.type() = 123;
		}
		Then(type_is_available) {
			AssertThrowsEx(UnexpectedItemTypeException, Root().cevent.type());
		}
	};
	When(set_action) {
		void SetUp() {
			Root().event.action() = "ACT";
		}
		Then(action_is_available) {
			Assert::That(Root().event.action().is_string(), Is().EqualTo(true));
			Assert::That(Root().event.action().as_string(), Is().EqualTo("ACT"));
			Assert::That(Root().cevent.action().as_string(), Is().EqualTo("ACT"));
		}
		Spec(check_diff) {
			property check;
			check["action"] = "ACT";
			Assert::That(Root().diff, Is().EqualTo(check));
		}
	};
	When(set_not_string_action) {
		void SetUp() {
			Root().event.action() = 123;
		}
		Then(action_is_available) {
			AssertThrowsEx(UnexpectedItemTypeException, Root().cevent.action());
		}
	};
};

