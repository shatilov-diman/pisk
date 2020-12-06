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
#include <pisk/utils/property_tree.h>
#include <pisk/utils/json_utils.h>

#include <functional>

using namespace igloo;
using namespace pisk::utils;
using namespace pisk::utils::json;

Context(property_access) {
	Context(const_none) {
		property _0;
		const property _1;
		Context(access_by_keystring) {
			Then(empty_returns) {
				Assert::That(Parent()._1["key"].is_none(), Is().EqualTo(true));
			}
			Then(size_is_zero) {
				Assert::That(Parent()._1.size(), Is().EqualTo(0U));
			}
			Then(container_is_none) {
				Assert::That(Parent()._1.is_none(), Is().EqualTo(true));
			}
		};
		Context(access_by_index) {
			Then(size_is_zero) {
				Assert::That(Parent()._1.size(), Is().EqualTo(0U));
			}
			Then(container_is_none) {
				Assert::That(Parent()._1.is_none(), Is().EqualTo(true));
			}
		};
	};
	Context(nonconst_none) {
		property _1;
		When(access_by_keystring) {
			void SetUp() {
				Parent()._1["key"];
			}
			Then(empty_returns) {
				Assert::That(Parent()._1["key"].is_none(), Is().EqualTo(true));
			}
			Then(size_is_one) {
				Assert::That(Parent()._1.size(), Is().EqualTo(1U));
			}
			Then(container_is_dictionary) {
				Assert::That(Parent()._1.is_dictionary(), Is().EqualTo(true));
			}
		};
		When(access_by_zero) {
			void SetUp() {
				Parent()._1[std::size_t(0)];
			}
			Then(out_of_range_still_occured) {
				AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, Parent()._1[std::size_t(-1)]);
				AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, Parent()._1[std::size_t(0x1000001)]);
			}
			Then(size_is_one) {
				Assert::That(Parent()._1.size(), Is().EqualTo(1U));
			}
			Then(container_is_array) {
				Assert::That(Parent()._1.is_array(), Is().EqualTo(true));
			}
		};
	};
	Context(check_contains) {
		property _1;
		Spec(for_none) {
			Assert::That(_1.contains("qwe"), Is().EqualTo(false));
		}
		Spec(for_bool) {
			_1 = true;
			AssertThrowsEx(PropertyCastException, _1.contains("qwe"));
		}
		Spec(for_int) {
			_1 = 1;
			AssertThrowsEx(PropertyCastException, _1.contains("qwe"));
		}
		Spec(for_string) {
			_1 = "qwe";
			AssertThrowsEx(PropertyCastException, _1.contains("qwe"));
		}
		Spec(for_array) {
			_1[std::size_t(0)] = "qwe";
			AssertThrowsEx(PropertyCastException, _1.contains("qwe"));
		}
		Spec(for_dictionary) {
			_1["qwe"] = "asd";
			Assert::That(_1.contains("qwe"), Is().EqualTo(true));
		}
	};
	When(access_by_out_of_index) {
		property _0;
		void SetUp() {
			_0[0x10] = 1;
		}
		Then(all_items_initialized_with_none) {
			for (std::size_t index = 0; index < 0x10; ++index)
				Assert::That(_0[index], Is().EqualTo(property::none_property()));
		}
		Then(accessed_item_available) {
			Assert::That(_0[0x10], Is().EqualTo(property {1}));
		}
		Then(size_increased_to_the_index) {
			Assert::That(_0.size(), Is().EqualTo(0x11U));
		}
		Then(size_does_not_icreased_to_much) {
			AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, _0[0x01000030]);
			AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, _0[0xffffffff]);
		}
	};
	When(const_access_by_out_of_index) {
		const property _0 { property::array {} };
		void SetUp() {
			_0[0x10];
		}
		Then(item_by_out_of_index_is_none) {
			Assert::That(_0[0xfffff0], Is().EqualTo(property::none_property()));
		}
		Then(size_is_not_increased) {
			Assert::That(_0.size(), Is().EqualTo(0U));
		}
		Then(out_of_index_access_is_not_too_much) {
			AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, _0[0x01000030]);
			AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, _0[0xffffffff]);
		}
	};
};

Context(from_string_to_string) {
	When(pass_none) {
		Then(none_returns) {
			Assert::That(parse_json_to_property(to_string(property {})), Is().EqualTo(property {}));
		}
	};
	When(pass_int) {
		Then(long_returns) {//jsoncpp restriction
			Assert::That(parse_json_to_property(to_string(property {7})), Is().EqualTo(property {7.}));
		}
	};
	When(pass_long) {
		Then(long_returns) {//jsoncpp restriction
			Assert::That(parse_json_to_property(to_string(property {8L})), Is().EqualTo(property {8.}));
		}
	};
	When(pass_float_without_decimal) {
		Then(long_returns) {//jsoncpp restriction
			Assert::That(parse_json_to_property(to_string(property {43.0f})), Is().EqualTo(property {43.}));
		}
	};
	When(pass_float) {
		Then(double_returns) {//jsoncpp restriction
			Assert::That(parse_json_to_property(to_string(property {41.1f})), Is().EqualTo(property {static_cast<double>(41.1f)}));
		}
	};
	When(pass_double) {
		Then(double_returns) {
			Assert::That(parse_json_to_property(to_string(property {42.2})), Is().EqualTo(property {42.2}));
		}
	};
	When(pass_string) {
		Then(string_returns) {
			Assert::That(parse_json_to_property(to_string(property {"qwe"})), Is().EqualTo(property {"qwe"}));
		}
	};
};

