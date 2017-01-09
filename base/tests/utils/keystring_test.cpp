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
#include <pisk/utils/keystring.h>

#include <functional>
#include <set>

using namespace igloo;
using namespace pisk::utils;

Describe(keystring_test) {
	std::set<std::string> generate_set() {
		std::set<std::string> out;
		for (std::size_t index = 0; index < 1000; ++index)
			out.insert(std::to_string(index));
		return out;
	}
	std::set<std::string> to_set(const std::set<keystring>& keys) {
			std::set<std::string> out;
			for (const keystring& str : keys)
				out.insert(*str);
			return out;
	}
	It(correct_for_empty) {
		const keystring empty1;
		const keystring empty2("");
		const keystring notempty("1");
		Assert::That(empty1.empty(), Is().EqualTo(true));
		Assert::That(empty2.empty(), Is().EqualTo(true));
		Assert::That(notempty.empty(), Is().EqualTo(false));
		Assert::That(empty1, Is().EqualTo(empty2));
	}
	Context(set_of_keystring) {
		std::set<keystring> keys;
		std::set<std::string> check = Root().generate_set();

		void SetUp() {
			for (const std::string& str : check)
				keys.insert(keystring(str));
		}

		Spec(its_same) {
			const std::set<std::string>& copy = Root().to_set(keys);
			Assert::That(copy, Is().EqualToContainer(check));
		}
	};
	Describe(size) {
		keystring _1;
		keystring _2 = keystring("");
		keystring _3 = keystring("a");
		keystring _4 = keystring("asdasd");
		keystring _5 = keystring(_4);
		It(size_1_is_zero) {
			Assert::That(_1.size(), Is().EqualTo(0U));
		}
		It(size_2_is_zero) {
			Assert::That(_2.size(), Is().EqualTo(0U));
		}
		It(size_3_is_zero) {
			Assert::That(_3.size(), Is().EqualTo(1U));
		}
		It(size_4_is_zero) {
			Assert::That(_4.size(), Is().EqualTo(6U));
		}
		It(size_5_is_zero) {
			Assert::That(_5.size(), Is().EqualTo(6U));
		}
	};
	Describe(compare) {
		keystring _1;
		keystring _2 = keystring("");
		keystring _3 = keystring("a");
		keystring _4 = keystring("asdasd");
		keystring _5 = keystring(_4);
		keystring _6 = keystring("asdasd");
		keystring _7 = keystring("ASDASD");
		It(_1_eq_2) {
			Assert::That(_1, Is().EqualTo(_2));
		}
		It(_2_neq_3) {
			Assert::That(_2, Is().Not().EqualTo(_3));
		}
		It(_3_neq_4) {
			Assert::That(_3, Is().Not().EqualTo(_4));
		}
		It(_4_eq_5) {
			Assert::That(_4, Is().EqualTo(_5));
		}
		It(_5_eq_6) {
			Assert::That(_5, Is().EqualTo(_6));
		}
		It(_6_eq_7) {
			Assert::That(_6, Is().Not().EqualTo(_7));
		}
	};
	When(get_content_for_default) {
		Then(returns_empty) {
			const keystring empty1;
			Assert::That(empty1.get_content(), Is().EqualTo(""));
		}
	};
	Context(concat_two_strings) {
		When(concat_two_keystrings) {
			Then(returns_concatanated_string) {
				const keystring _1("qwe");
				const keystring _2("asd");
				Assert::That(_1 + _2, Is().EqualTo(keystring {"qweasd"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"qweasd"}.get_hash()));
			}
		};
		When(concat_with_null_right) {
			Then(returns_concatanated_string) {
				const keystring _1("qwe");
				const char* _2 = nullptr;
				Assert::That(_1 + _2, Is().EqualTo(keystring {"qwe"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"qwe"}.get_hash()));
			}
		};
		When(concat_with_null_left) {
			Then(returns_concatanated_string) {
				const char* _1 = nullptr;
				const keystring _2("asd");
				Assert::That(_1 + _2, Is().EqualTo(keystring {"asd"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"asd"}.get_hash()));
			}
		};
		When(concat_with_empty_right) {
			Then(returns_concatanated_string) {
				const keystring _1("qwe");
				Assert::That(_1 + "", Is().EqualTo(keystring {"qwe"}));
				Assert::That((_1 + "").get_hash(), Is().EqualTo(keystring {"qwe"}.get_hash()));
			}
		};
		When(concat_with_empty_left) {
			Then(returns_concatanated_string) {
				const keystring _2("asd");
				Assert::That("" + _2, Is().EqualTo(keystring {"asd"}));
				Assert::That(("" + _2).get_hash(), Is().EqualTo(keystring {"asd"}.get_hash()));
			}
		};
		When(concat_with_cstring_right) {
			Then(returns_concatanated_string) {
				const keystring _1("qwe");
				const char* _2 = "asd";
				Assert::That(_1 + _2, Is().EqualTo(keystring {"qweasd"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"qweasd"}.get_hash()));
			}
		};
		When(concat_with_cstring_left) {
			Then(returns_concatanated_string) {
				const char* _1 = "qwe";
				const keystring _2("asd");
				Assert::That(_1 + _2, Is().EqualTo(keystring {"qweasd"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"qweasd"}.get_hash()));
			}
		};
		When(concat_with_chararry_right) {
			Then(returns_concatanated_string) {
				const keystring _1("qwe");
				const char _2[] = "asd";
				Assert::That(_1 + _2, Is().EqualTo(keystring {"qweasd"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"qweasd"}.get_hash()));
			}
		};
		When(concat_with_chararry_left) {
			Then(returns_concatanated_string) {
				const char _1[] = "qwe";
				const keystring _2("asd");
				Assert::That(_1 + _2, Is().EqualTo(keystring {"qweasd"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"qweasd"}.get_hash()));
			}
		};
		When(concat_with_std_string_right) {
			Then(returns_concatanated_string) {
				const keystring _1("qwe");
				const std::string _2 = "asd";
				Assert::That(_1 + _2, Is().EqualTo(keystring {"qweasd"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"qweasd"}.get_hash()));
			}
		};
		When(concat_with_std_string_left) {
			Then(returns_concatanated_string) {
				const std::string _1 = "qwe";
				const keystring _2("asd");
				Assert::That(_1 + _2, Is().EqualTo(keystring {"qweasd"}));
				Assert::That((_1 + _2).get_hash(), Is().EqualTo(keystring {"qweasd"}.get_hash()));
			}
		};
	};
	When(compare_two_strings) {
		When(compare_with_null) {
			Then(compared) {
				const keystring _1;
				const keystring _2("qwe");
				const char* _3 = nullptr;
				Assert::That(_1 == _3, Is().EqualTo(true));
				Assert::That(_1 != _3, Is().EqualTo(false));
				Assert::That(_2 == _3, Is().EqualTo(false));
				Assert::That(_2 != _3, Is().EqualTo(true));
				Assert::That(_3 == _1, Is().EqualTo(true));
				Assert::That(_3 != _1, Is().EqualTo(false));
				Assert::That(_3 == _2, Is().EqualTo(false));
				Assert::That(_3 != _2, Is().EqualTo(true));
			}
		};
		When(compare_with_empty_cstr_right) {
			Then(compared) {
				const keystring _1;
				const keystring _2("qwe");
				Assert::That(_1 == "", Is().EqualTo(true));
				Assert::That(_1 != "", Is().EqualTo(false));
				Assert::That(_2 == "", Is().EqualTo(false));
				Assert::That(_2 != "", Is().EqualTo(true));
			}
		};
		When(compare_with_empty_cstr_left) {
			Then(compared) {
				const keystring _1;
				const keystring _2("qwe");
				Assert::That("" == _1, Is().EqualTo(true));
				Assert::That("" != _1, Is().EqualTo(false));
				Assert::That("" == _2, Is().EqualTo(false));
				Assert::That("" != _2, Is().EqualTo(true));
			}
		};
		When(compare_two_keystrings) {
			Then(compared) {
				const keystring _1("qwe");
				const keystring _2("qwe");
				const keystring _3("asd");
				Assert::That(_1 == _1, Is().EqualTo(true));
				Assert::That(_1 == _2, Is().EqualTo(true));
				Assert::That(_1 == _3, Is().EqualTo(false));
				Assert::That(_1 != _1, Is().EqualTo(false));
				Assert::That(_1 != _2, Is().EqualTo(false));
				Assert::That(_1 != _3, Is().EqualTo(true));
			}
		};
		When(concat_two_cstring_right) {
			Then(compared) {
				const keystring _1("qwe");
				const char* _2 = "qwe";
				const char* _3 = "asd";
				Assert::That(_1 == _1, Is().EqualTo(true));
				Assert::That(_1 == _2, Is().EqualTo(true));
				Assert::That(_1 == _3, Is().EqualTo(false));
				Assert::That(_1 != _1, Is().EqualTo(false));
				Assert::That(_1 != _2, Is().EqualTo(false));
				Assert::That(_1 != _3, Is().EqualTo(true));
			}
		};
		When(concat_two_cstring_left) {
			Then(compared) {
				const keystring _1("qwe");
				const char* _2 = "qwe";
				const char* _3 = "asd";
				Assert::That(_1 == _1, Is().EqualTo(true));
				Assert::That(_2 == _1, Is().EqualTo(true));
				Assert::That(_3 == _1, Is().EqualTo(false));
				Assert::That(_1 != _1, Is().EqualTo(false));
				Assert::That(_2 != _1, Is().EqualTo(false));
				Assert::That(_3 != _1, Is().EqualTo(true));
			}
		};
		When(concat_two_chararry_right) {
			Then(compared) {
				const keystring _1("qwe");
				const char _2[] = "qwe";
				const char _3[] = "asd";
				Assert::That(_1 == _1, Is().EqualTo(true));
				Assert::That(_1 == _2, Is().EqualTo(true));
				Assert::That(_1 == _3, Is().EqualTo(false));
				Assert::That(_1 != _1, Is().EqualTo(false));
				Assert::That(_1 != _2, Is().EqualTo(false));
				Assert::That(_1 != _3, Is().EqualTo(true));
			}
		};
		When(concat_two_chararry_left) {
			Then(compared) {
				const keystring _1("qwe");
				const char _2[] = "qwe";
				const char _3[] = "asd";
				Assert::That(_1 == _1, Is().EqualTo(true));
				Assert::That(_2 == _1, Is().EqualTo(true));
				Assert::That(_3 == _1, Is().EqualTo(false));
				Assert::That(_1 != _1, Is().EqualTo(false));
				Assert::That(_2 != _1, Is().EqualTo(false));
				Assert::That(_3 != _1, Is().EqualTo(true));
			}
		};
		When(compare_with_empty_std_string_right) {
			Then(compared) {
				const keystring _1("qwe");
				const std::string _2("qwe");
				const std::string _3("asd");
				Assert::That(_1 == _1, Is().EqualTo(true));
				Assert::That(_1 == _2, Is().EqualTo(true));
				Assert::That(_1 == _3, Is().EqualTo(false));
				Assert::That(_1 != _1, Is().EqualTo(false));
				Assert::That(_1 != _2, Is().EqualTo(false));
				Assert::That(_1 != _3, Is().EqualTo(true));
			}
		};
		When(compare_with_empty_std_string_left) {
			Then(compared) {
				const keystring _1("qwe");
				const std::string _2("qwe");
				const std::string _3("asd");
				Assert::That(_1 == _1, Is().EqualTo(true));
				Assert::That(_2 == _1, Is().EqualTo(true));
				Assert::That(_3 == _1, Is().EqualTo(false));
				Assert::That(_1 != _1, Is().EqualTo(false));
				Assert::That(_2 != _1, Is().EqualTo(false));
				Assert::That(_3 != _1, Is().EqualTo(true));
			}
		};
	};

	Describe(compatibility_with_std_algorithm) {
		It(can_copy_empty_string) {
			const keystring _1;
			std::string dest;
			std::copy(_1.begin(), _1.end(), std::back_inserter(dest));
			Assert::That(dest, Is().EqualTo(""));
		}
		It(can_copy_string) {
			const keystring _1("qwe");
			std::string dest;
			std::copy(_1.begin(), _1.end(), std::back_inserter(dest));
			Assert::That(dest, Is().EqualTo("qwe"));
		}
	};
};

