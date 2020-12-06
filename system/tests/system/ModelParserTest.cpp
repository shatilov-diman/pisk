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
