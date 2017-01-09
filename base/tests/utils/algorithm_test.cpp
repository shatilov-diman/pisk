// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
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
#include <pisk/utils/algorithm_utils.h>

#include <functional>

using namespace igloo;
using namespace pisk::utils;

const std::vector<std::string> joindata0;
const std::vector<std::string> joindata1 = {"hello", "world"};

Describe(algorithm_join) {
	When(join_empty) {
		Then(its_empty) {
			std::string result = algorithm::join(joindata0, std::string());
			Assert::That(result, Is().EqualTo(std::string()));
		}
	};
	When(join_hello_and_world_without_linker) {
		Then(its_equal_to_helloworld) {
			std::string result = algorithm::join(joindata1, std::string());
			Assert::That(result, Is().EqualTo(std::string("helloworld")));
		}
	};
	When(join_hello_and_world_with_space) {
		Then(its_equal_to_helloworld) {
			std::string result = algorithm::join(joindata1, std::string(" "));
			Assert::That(result, Is().EqualTo(std::string("hello world")));
		}
	};
	When(join_list) {
		Then(its_joined) {
			std::string result = algorithm::join(joindata1, std::string(" "));
			Assert::That(result, Is().EqualTo(std::string("hello world")));
		}
	};
	When(join_wide_strins) {
		Then(its_joined) {
			std::wstring result = algorithm::join(joindata1, std::wstring(L" "));
			Assert::That(result, Is().EqualTo(std::wstring(L"hello world")));
		}
	};
};

Describe(format) {
	When(format_without_args) {
		Then(its_simple_string) {
			const std::string& result = string::format(std::string("simple"));
			Assert::That(result, Is().EqualTo(std::string("simple")));
		}
	};
	When(format_with_args) {
		Then(its_expected) {
			const std::string& result = string::format(std::string("simple %s %d"), "xxx", 15);
			Assert::That(result, Is().EqualTo(std::string("simple xxx 15")));
		}
	};
};
