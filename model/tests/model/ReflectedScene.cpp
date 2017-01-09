// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module model of the project pisk.
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

#include <pisk/model/ReflectedScene.h>

using namespace igloo;
using namespace pisk::utils;
using namespace pisk::model;

static std::size_t calculate_events(const ReflectedScene& scene) {
	std::size_t count = 0;
	for (auto event : scene.events())
	{
		UNUSED(event);
		++count;
	}
	return count;
}

Describe(TestReflectedScene) {
	property orig;
	property diff;

	ReflectedScene scene{orig, diff};
	const ReflectedScene& cscene = scene;

	Spec(orig_and_diff_are_none) {
		property check;
		Assert::That(Root().orig, Is().EqualTo(check));
		Assert::That(Root().diff, Is().EqualTo(check));
	}
	Context(empty_scene) {
		void SetUp() {
			Root().scene.id() = "root";
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["properties"]["id"] = "root";
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		Spec(foreach_event) {
			Assert::That(calculate_events(Root().scene), Is().EqualTo(0U));
		}
	};
	When(add_event) {
		void SetUp() {
			auto event = Root().scene.push_event<ReflectedEvent>();
			event.source() = "XXX";
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["events"][std::size_t(0)]["source"] = "XXX";
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		Spec(event_is_available) {
			auto iter = Root().scene.events().begin();
			auto end = Root().scene.events().end();
			auto event = *iter;
			Assert::That(iter, Is().Not().EqualTo(end));
			Assert::That(event.source().as_string(), Is().EqualTo("XXX"));
		}
		Spec(foreach_event) {
			Assert::That(calculate_events(Root().scene), Is().EqualTo(1U));
		}
	};
};

