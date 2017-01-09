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
#include "../../sources/system/ModelParser.h"

using namespace igloo;
using namespace pisk;

Describe(ModelParserTest) {
	Context(empty_scene) {
		utils::property desc;
		Spec(no_submodels) {
			Assert::That(system::ModelParser::parse(desc), Is().Empty());
		}
	};
	Context(empty_properties) {
		utils::property desc;
		void SetUp() {
			desc["properties"] = utils::property::dictionary {};
		}
		Spec(no_submodels) {
			Assert::That(system::ModelParser::parse(desc).size(), Is().EqualTo(1U));
			Assert::That(system::ModelParser::parse(desc).begin()->first.get_content(), Is().EqualTo("properties"));
		}
	};
	Context(objects_as_int) {
		utils::property desc;
		void SetUp() {
			desc["objects"] = 123;
		}
		Spec(no_submodels) {
			Assert::That(system::ModelParser::parse(desc), Is().Empty());
		}
	};
	Context(submodels_in_array) {
		utils::property desc;
		void SetUp() {
			desc[std::size_t(0)]["location"]["model"] = "none";
		}
		Spec(no_submodels) {
			Assert::That(system::ModelParser::parse(desc), Is().Empty());
		}
	};
	Context(submodel_as_array) {
		utils::property desc;
		void SetUp() {
			desc["audio"] = utils::property::array {};
		}
		Spec(no_submodels) {
			Assert::That(system::ModelParser::parse(desc), Is().Empty());
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
		Spec(no_submodels) {
			Assert::That(system::ModelParser::parse(desc).size(), Is().EqualTo(1U));
			Assert::That(system::ModelParser::parse(desc)[utils::keystring("properties")]["name"].as_string(), Is().EqualTo("Level 0"));
			Assert::That(system::ModelParser::parse(desc)[utils::keystring("properties")]["author"].as_string(), Is().EqualTo("KG/AM"));
		}
	};
	Context(correct_model) {
		utils::property desc;
		void SetUp() {
			desc["properties"]["name"] = "Duck";
			desc["audio"]["rid"] = "some_audio.ogg";
			desc["script"]["rid"] = "SuperUI.lua";
		}
		Spec(check_data) {
			Assert::That(system::ModelParser::parse(desc).size(), Is().EqualTo(3U));
			Assert::That(system::ModelParser::parse(desc)[utils::keystring("properties")]["name"].as_string(), Is().EqualTo("Duck"));
			Assert::That(system::ModelParser::parse(desc)[utils::keystring("audio")]["rid"].as_string(), Is().EqualTo("some_audio.ogg"));
			Assert::That(system::ModelParser::parse(desc)[utils::keystring("script")]["rid"].as_string(), Is().EqualTo("SuperUI.lua"));
		}
	};
};
