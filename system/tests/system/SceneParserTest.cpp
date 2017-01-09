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
#include "../../sources/system/SceneParser.h"

using namespace igloo;
using namespace pisk;

//TODO: json_utils_test?

Describe(SceneParserTest) {
	Context(empty_scene) {
		utils::property desc;
		Spec(no_objects) {
			Assert::That(system::SceneParser::parse(desc), Is().Empty());
		}
	};
	Context(empty_properties) {
		utils::property desc;
		void SetUp() {
			desc["properties"] = utils::property::dictionary {};
		}
		Spec(no_objects) {
			Assert::That(system::SceneParser::parse(desc), Is().Empty());
		}
	};
	Context(objects_as_int) {
		utils::property desc;
		void SetUp() {
			desc["objects"] = 123;
		}
		Spec(no_objects) {
			Assert::That(system::SceneParser::parse(desc), Is().Empty());
		}
	};
	Context(objects_as_dictionary) {
		utils::property desc;
		void SetUp() {
			desc["objects"]["1"]["model"] = "none";
		}
		Spec(no_objects) {
			Assert::That(system::SceneParser::parse(desc), Is().Empty());
		}
	};
	Context(empty_objects) {
		utils::property desc;
		void SetUp() {
			desc["objects"] = utils::property::array {};
		}
		Spec(no_objects) {
			Assert::That(system::SceneParser::parse(desc), Is().Empty());
		}
	};
	Context(propertirs_only) {
		utils::property desc;
		void SetUp() {
			desc["properties"] = utils::property::dictionary {
				std::make_pair(utils::keystring("name"), utils::property("Level 0")),
				std::make_pair(utils::keystring("author"), utils::property("KG/AM")),
			};
		}
		Spec(no_objects) {
			Assert::That(system::SceneParser::parse(desc), Is().Empty());
			Assert::That(desc["properties"]["name"].as_string(), Is().EqualTo("Level 0"));
			Assert::That(desc["properties"]["author"].as_string(), Is().EqualTo("KG/AM"));
		}
	};
	Context(one_object_and_model_only) {
		utils::property desc;
		void SetUp() {
			desc["objects"][std::size_t()] = utils::property::dictionary {
				std::make_pair(utils::keystring("model"), utils::property("some_model")),
			};
		}
		Spec(size_is_one__name_is_some_model) {
			Assert::That(system::SceneParser::parse(desc).size(), Is().EqualTo(1U));
			Assert::That(system::SceneParser::parse(desc)[0].first.get_content(), Is().EqualTo("some_model"));
			Assert::That(system::SceneParser::parse(desc)[0].second.is_none(), Is().EqualTo(true));
		}
	};
	Context(two_object_with_overrides) {
		utils::property desc;
		void SetUp() {
			desc["objects"][std::size_t(0)] = utils::property::dictionary {
				std::make_pair(utils::keystring("model"), utils::property("some_model")),
				std::make_pair(utils::keystring("overrides"), utils::property(utils::property::dictionary {
					std::make_pair(utils::keystring("location"), utils::property("1,1,1")),
				})),
			};
			desc["objects"][std::size_t(1)] = utils::property::dictionary {
				std::make_pair(utils::keystring("model"), utils::property("another_model")),
			};
		}
		Spec(check_objects) {
			Assert::That(system::SceneParser::parse(desc).size(), Is().EqualTo(2U));
			Assert::That(system::SceneParser::parse(desc)[0].first.get_content(), Is().EqualTo("some_model"));
			Assert::That(system::SceneParser::parse(desc)[0].second["location"].as_string(), Is().EqualTo("1,1,1"));
			Assert::That(system::SceneParser::parse(desc)[1].first.get_content(), Is().EqualTo("another_model"));
			Assert::That(system::SceneParser::parse(desc)[1].second.is_none(), Is().EqualTo(true));
		}
	};
};

