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

#include <pisk/model/location/ReflectedPresentation.h>

using namespace igloo;
using namespace pisk::utils;
using namespace pisk::model;

Describe(TestReflectedLocationPresentation) {
	property orig;
	property diff;

	location::Presentation presentation{orig, diff};

	Spec(presentation_id_is_location) {
		Assert::That(location::Presentation::presentation_id(), Is().EqualTo("location"));
	}

	Spec(orig_and_diff_are_none) {
		property check;
		Assert::That(Root().orig, Is().EqualTo(check));
		Assert::That(Root().diff, Is().EqualTo(check));
	}
	Context(check_position) {
		void SetUp() {
			Root().presentation.position().x() = 1.;
			Root().presentation.position().y() = 2.;
			Root().presentation.position().z() = 3.;
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["properties"]["position"]["x"] = 1.;
			check["properties"]["position"]["y"] = 2.;
			check["properties"]["position"]["z"] = 3.;
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		Spec(check_cref) {
			Assert::That(Root().presentation.position().const_ref().x().as_double(), Is().EqualTo(1.));
			Assert::That(Root().presentation.position().const_ref().y().as_double(), Is().EqualTo(2.));
			Assert::That(Root().presentation.position().const_ref().z().as_double(), Is().EqualTo(3.));
		}
	};
	Context(check_quaternion) {
		void SetUp() {
			Root().presentation.quaternion().x() = 4.;
			Root().presentation.quaternion().y() = 5.;
			Root().presentation.quaternion().z() = 6.;
			Root().presentation.quaternion().w() = 7.;
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["properties"]["quaternion"]["x"] = 4.;
			check["properties"]["quaternion"]["y"] = 5.;
			check["properties"]["quaternion"]["z"] = 6.;
			check["properties"]["quaternion"]["w"] = 7.;
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		Spec(check_cref) {
			Assert::That(Root().presentation.quaternion().const_ref().x().as_double(), Is().EqualTo(4.));
			Assert::That(Root().presentation.quaternion().const_ref().y().as_double(), Is().EqualTo(5.));
			Assert::That(Root().presentation.quaternion().const_ref().z().as_double(), Is().EqualTo(6.));
			Assert::That(Root().presentation.quaternion().const_ref().w().as_double(), Is().EqualTo(7.));
		}
	};
	Context(check_scale) {
		void SetUp() {
			Root().presentation.scale().x() = 8.;
			Root().presentation.scale().y() = 9.;
			Root().presentation.scale().z() = 10.;
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["properties"]["scale"]["x"] = 8.;
			check["properties"]["scale"]["y"] = 9.;
			check["properties"]["scale"]["z"] = 10.;
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		Spec(check_cref) {
			Assert::That(Root().presentation.scale().const_ref().x().as_double(), Is().EqualTo(8.));
			Assert::That(Root().presentation.scale().const_ref().y().as_double(), Is().EqualTo(9.));
			Assert::That(Root().presentation.scale().const_ref().z().as_double(), Is().EqualTo(10.));
		}
	};
	Context(check_position_and_quaternion) {
		void SetUp() {
			Root().presentation.position().x() = 1.;
			Root().presentation.position().y() = 2.;
			Root().presentation.position().z() = 3.;
			Root().presentation.quaternion().x() = 4.;
			Root().presentation.quaternion().y() = 5.;
			Root().presentation.quaternion().z() = 6.;
			Root().presentation.quaternion().w() = 7.;
			Root().presentation.scale().x() = 8.;
			Root().presentation.scale().y() = 9.;
			Root().presentation.scale().z() = 10.;
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["properties"]["position"]["x"] = 1.;
			check["properties"]["position"]["y"] = 2.;
			check["properties"]["position"]["z"] = 3.;
			check["properties"]["quaternion"]["x"] = 4.;
			check["properties"]["quaternion"]["y"] = 5.;
			check["properties"]["quaternion"]["z"] = 6.;
			check["properties"]["quaternion"]["w"] = 7.;
			check["properties"]["scale"]["x"] = 8.;
			check["properties"]["scale"]["y"] = 9.;
			check["properties"]["scale"]["z"] = 10.;
			Assert::That(Root().diff, Is().EqualTo(check));
		}
	};
};

