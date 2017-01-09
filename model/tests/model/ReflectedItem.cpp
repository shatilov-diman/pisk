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

#include <pisk/model/ReflectedItem.h>

#include <limits>

using namespace igloo;
using namespace pisk::utils;
using namespace pisk::model;

Describe(TestReflectedItem) {
	property orig;
	property diff;

	ReflectedItem item{orig, diff};

	Spec(item_is_none) {
		Assert::That(item.is_none(), Is().EqualTo(true));
		Assert::That(item.is_bool(), Is().EqualTo(false));
	}
	Spec(item_has_not_changes) {
		Assert::That(item.has_origin(), Is().EqualTo(false));
		Assert::That(item.has_changes(), Is().EqualTo(false));
	}
	Spec(size_is_zero) {
		Assert::That(item.size(), Is().EqualTo(0U));
	}
	Spec(item_as_type_thrown_exception) {
		AssertThrowsEx(PropertyCastException, item.as_bool());
		AssertThrowsEx(PropertyCastException, item.as_int());
		AssertThrowsEx(PropertyCastException, item.as_string());
	}
	Spec(item_not_eq_to_int) {
		Assert::That(item == 2, Is().EqualTo(false));
	}
	Spec(item_not_eq_to_cstring) {
		Assert::That(item == "key", Is().EqualTo(false));
	}
	When(origin_has_data) {
		void SetUp() {
			ReflectedItem edit{property::none_property(), Root().orig};
			edit.get_int_item("asd") = 15;
			edit.get_string_item("qwe") = "zxc";
		}
		Then(item_has_origin) {
			Assert::That(Root().item.has_origin(), Is().EqualTo(true));
		}
		Then(item_has_not_changes) {
			Assert::That(Root().item.has_changes(), Is().EqualTo(false));
		}
		Then(check_data) {
			Assert::That(Root().item.get_int_item("asd"), Is().EqualTo(15));
			Assert::That(Root().item.get_string_item("qwe"), Is().EqualTo("zxc"));
		}
		When(change_item) {
			void SetUp() {
				Root().item.get_int_item("asd") = "rty";
			}
			Then(check_data) {
				Assert::That(Root().item.origin().get_int_item("asd"), Is().EqualTo(15));
				Assert::That(Root().item.get_string_item("asd"), Is().EqualTo("rty"));
				Assert::That(Root().item.get_string_item("qwe"), Is().EqualTo("zxc"));
			}
			Then(item_has_origin) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(true));
			}
			Then(item_has_changes) {
				Assert::That(Root().item.has_changes(), Is().EqualTo(true));
			}
		};
	};
	When(property_init_with_bool) {
		Then(item_is_bool) {
			Root().diff = true;
			Assert::That(Root().item.is_bool(), Is().EqualTo(true));
			Assert::That(Root().item.as_bool(), Is().EqualTo(true));
			AssertThrowsEx(PropertyCastException, Root().item.size());
		}
	};
	When(property_init_with_int) {
		Then(item_is_int) {
			Root().diff = 7;
			Assert::That(Root().item.is_int(), Is().EqualTo(true));
			Assert::That(Root().item.as_int(), Is().EqualTo(7));
			AssertThrowsEx(PropertyCastException, Root().item.size());
		}
	};
	When(property_init_with_long) {
		Then(item_is_long) {
			Root().diff = std::numeric_limits<long>::max();
			Assert::That(Root().item.is_long(), Is().EqualTo(true));
			Assert::That(Root().item.as_long(), Is().EqualTo(std::numeric_limits<long>::max()));
			AssertThrowsEx(PropertyCastException, Root().item.size());
		}
	};
	When(property_init_with_float) {
		Then(item_is_float) {
			Root().diff = 0.123f;
			Assert::That(Root().item.is_float(), Is().EqualTo(true));
			Assert::That(Root().item.as_float(), Is().EqualTo(0.123f));
			AssertThrowsEx(PropertyCastException, Root().item.size());
		}
	};
	When(property_init_with_double) {
		Then(item_is_double) {
			Root().diff = std::numeric_limits<double>::max();
			Assert::That(Root().item.is_double(), Is().EqualTo(true));
			Assert::That(Root().item.as_double(), Is().EqualTo(std::numeric_limits<double>::max()));
			AssertThrowsEx(PropertyCastException, Root().item.size());
		}
	};
	When(property_init_with_string) {
		Then(item_is_string) {
			Root().diff = "qwe";
			Assert::That(Root().item.is_string(), Is().EqualTo(true));
			Assert::That(Root().item.as_string(), Is().EqualTo("qwe"));
			Assert::That(Root().item.as_keystring().c_str(), Is().EqualTo("qwe"));
			Assert::That(Root().item.size(), Is().EqualTo(3U));
		}
	};
	When(property_init_with_array) {
		Then(item_is_array) {
			Root().diff[std::size_t(0)];
			Assert::That(Root().item.is_array(), Is().EqualTo(true));
			Assert::That(Root().item.size(), Is().EqualTo(1U));
		}
	};
	When(property_init_with_dictionary) {
		const ReflectedItem& citem = Root().item;
		void SetUp() {
			Root().diff["1"] = nullptr;
			Root().diff["2"] = true;
			Root().diff["3"] = 15;
			Root().diff["4"] = "asd";
			Root().diff["5"]["6"] = 123;
			Root().diff["7"][std::size_t(0)] = 15;
		}
		Then(item_is_dictionary) {
			Assert::That(Root().item.is_dictionary(), Is().EqualTo(true));
			Assert::That(citem.is_dictionary(), Is().EqualTo(true));
			Assert::That(Root().item.size(), Is().EqualTo(6U));
		}
		Spec(get_bool_item) {
			Assert::That(Root().item.get_bool_item("2").as_bool(), Is().EqualTo(true));
			Assert::That(citem.get_bool_item("2").as_bool(), Is().EqualTo(true));
		}
		Spec(get_int_item) {
			Assert::That(Root().item.get_int_item("3").as_int(), Is().EqualTo(15));
			Assert::That(citem.get_int_item("3").as_int(), Is().EqualTo(15));
		}
		Spec(get_string_item) {
			Assert::That(Root().item.get_string_item("4").as_string(), Is().EqualTo("asd"));
			Assert::That(citem.get_string_item("4").as_string(), Is().EqualTo("asd"));
		}
		Spec(get_dictionary_item) {
			Assert::That(Root().item.get_dictionary_item("5").is_dictionary(), Is().EqualTo(true));
			Assert::That(citem.get_dictionary_item("5").is_dictionary(), Is().EqualTo(true));
		}
		Spec(get_array_item) {
			Assert::That(Root().item.get_array_item("7").is_array(), Is().EqualTo(true));
			Assert::That(citem.get_array_item("7").is_array(), Is().EqualTo(true));
		}
		Spec(get_custom_item) {
			Assert::That(Root().item.get_custom_item<ReflectedItem>("3").is_int(), Is().EqualTo(true));
			Assert::That(citem.get_custom_item<ConstReflectedItem>("3").is_int(), Is().EqualTo(true));
		}
		Spec(get_incorrect_item_type) {
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("2"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_bool_item("3"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("4"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("5"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("7"));
		}
		Spec(get_bool_for_none) {
			Assert::That(Root().item.get_bool_item("!1").is_none(), Is().EqualTo(true));
			Root().item.get_bool_item("!1") = true;
			Assert::That(Root().item.get_bool_item("!1").as_bool(), Is().EqualTo(true));
		}
		Spec(get_int_for_none) {
			Assert::That(Root().item.get_int_item("!2").is_none(), Is().EqualTo(true));
			Root().item.get_int_item("!2") = 17;
			Assert::That(Root().item.get_int_item("!2").as_int(), Is().EqualTo(17));
		}
		Spec(get_string_for_none) {
			Assert::That(Root().item.get_string_item("!3").is_none(), Is().EqualTo(true));
			Root().item.get_string_item("!3") = "asd";
			Assert::That(Root().item.get_string_item("!3").as_string(), Is().EqualTo("asd"));
		}
		Spec(get_dictionary_for_none) {
			Assert::That(Root().item.get_dictionary_item("!4").is_none(), Is().EqualTo(true));
			Root().item.get_dictionary_item("!4") = property(property::dictionary {});
			Assert::That(Root().item.get_dictionary_item("!4").is_dictionary(), Is().EqualTo(true));
		}
		Spec(get_array_for_none) {
			Assert::That(Root().item.get_array_item("!5").is_none(), Is().EqualTo(true));
			Root().item.get_array_item("!5") = property(property::array {});
			Assert::That(Root().item.get_array_item("!5").is_array(), Is().EqualTo(true));
		}
	};
	When(assign_int) {
		void SetUp() {
			Root().item = 146;
		}
		Then(item_has_changes) {
			Assert::That(Root().item.has_origin(), Is().EqualTo(false));
			Assert::That(Root().item.has_changes(), Is().EqualTo(true));
		}
		Then(item_is_int) {
			Assert::That(Root().item.is_int(), Is().EqualTo(true));
			Assert::That(Root().item.as_int(), Is().EqualTo(146));
		}
		Then(original_still_none) {
			Assert::That(Root().item.origin().is_none(), Is().EqualTo(true));
		}
		When(clear) {
			void SetUp() {
				Root().item.clear();
			}
			Then(item_is_none) {
				Assert::That(Root().item.is_none(), Is().EqualTo(true));
			}
			Then(original_still_none) {
				Assert::That(Root().item.origin().is_none(), Is().EqualTo(true));
			}
		};
		When(reassign_string) {
			void SetUp() {
				Root().item = "qwe";
			}
			Then(item_has_changes) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(false));
				Assert::That(Root().item.has_changes(), Is().EqualTo(true));
			}
			Then(item_is_string) {
				Assert::That(Root().item.is_string(), Is().EqualTo(true));
				Assert::That(Root().item.as_string(), Is().EqualTo("qwe"));
			}
			Then(original_still_none) {
				Assert::That(Root().item.origin().is_none(), Is().EqualTo(true));
			}
		};
	};
	When(assign_array) {
		void SetUp() {
			Root().item = property::array {
				{std::size_t(0), nullptr},
				{std::size_t(1), "key"},
				{std::size_t(2), 15.f},
			};
		}
		Then(item_has_changes) {
			Assert::That(Root().item.has_origin(), Is().EqualTo(false));
			Assert::That(Root().item.has_changes(), Is().EqualTo(true));
		}
		Then(item_is_array) {
			Assert::That(Root().item.is_array(), Is().EqualTo(true));
		}
		Then(original_still_none) {
			Assert::That(Root().item.origin().is_none(), Is().EqualTo(true));
		}
	};
	When(assign_dictionary) {
		void SetUp() {
			Root().item = property::dictionary {
				{"key_1", nullptr},
				{"key_2", "key"},
				{"key_3", 15.f},
			};
		}
		Then(item_has_changes) {
			Assert::That(Root().item.has_origin(), Is().EqualTo(false));
			Assert::That(Root().item.has_changes(), Is().EqualTo(true));
		}
		Then(item_is_dictionary) {
			Assert::That(Root().item.is_dictionary(), Is().EqualTo(true));
		}
		Then(original_still_none) {
			Assert::That(Root().item.origin().is_none(), Is().EqualTo(true));
		}
	};
	Context(compare_bool) {
		void SetUp() {
			Root().item = true;
		}
		Spec(with_bool) {
			Assert::That(Root().item, Is().EqualTo(true));
			Assert::That(Root().item, Is().Not().EqualTo(false));
		}
		Spec(with_int) {
			Assert::That(Root().item, Is().Not().EqualTo(5));
		}
		Spec(with_cstring) {
			Assert::That(Root().item, Is().Not().EqualTo("qwe"));
		}
		Spec(with_string) {
			Assert::That(Root().item, Is().Not().EqualTo(std::string {"qwe"}));
		}
		Spec(with_keystring) {
			Assert::That(Root().item, Is().Not().EqualTo(keystring {"qwe"}));
		}
	};
	Context(compare_int) {
		void SetUp() {
			Root().item = 5;
		}
		Spec(with_bool) {
			Assert::That(Root().item, Is().Not().EqualTo(false));
		}
		Spec(with_int) {
			Assert::That(Root().item, Is().EqualTo(5));
			Assert::That(Root().item, Is().Not().EqualTo(25));
		}
		Spec(with_long) {
			Assert::That(Root().item, Is().Not().EqualTo(5L));
		}
		Spec(with_cstring) {
			Assert::That(Root().item, Is().Not().EqualTo("qwe"));
		}
		Spec(with_string) {
			Assert::That(Root().item, Is().Not().EqualTo(std::string {"qwe"}));
		}
		Spec(with_keystring) {
			Assert::That(Root().item, Is().Not().EqualTo(keystring {"qwe"}));
		}
	};
	Context(compare_long) {
		void SetUp() {
			Root().item = 5L;
		}
		Spec(with_bool) {
			Assert::That(Root().item, Is().Not().EqualTo(false));
		}
		Spec(with_int) {
			Assert::That(Root().item, Is().Not().EqualTo(5));
		}
		Spec(with_long) {
			Assert::That(Root().item, Is().EqualTo(5L));
			Assert::That(Root().item, Is().Not().EqualTo(25L));
		}
		Spec(with_cstring) {
			Assert::That(Root().item, Is().Not().EqualTo("qwe"));
		}
		Spec(with_string) {
			Assert::That(Root().item, Is().Not().EqualTo(std::string {"qwe"}));
		}
		Spec(with_keystring) {
			Assert::That(Root().item, Is().Not().EqualTo(keystring {"qwe"}));
		}
	};
	Context(compare_float) {
		void SetUp() {
			Root().item = 5.f;
		}
		Spec(with_bool) {
			Assert::That(Root().item, Is().Not().EqualTo(false));
		}
		Spec(with_int) {
			Assert::That(Root().item, Is().Not().EqualTo(5));
		}
		Spec(with_float) {
			Assert::That(Root().item, Is().EqualTo(5.f));
			Assert::That(Root().item, Is().Not().EqualTo(25.f));
		}
		Spec(with_double) {
			Assert::That(Root().item, Is().Not().EqualTo(5.));
		}
		Spec(with_cstring) {
			Assert::That(Root().item, Is().Not().EqualTo("qwe"));
		}
		Spec(with_string) {
			Assert::That(Root().item, Is().Not().EqualTo(std::string {"qwe"}));
		}
		Spec(with_keystring) {
			Assert::That(Root().item, Is().Not().EqualTo(keystring {"qwe"}));
		}
	};
	Context(compare_double) {
		void SetUp() {
			Root().item = 5.;
		}
		Spec(with_bool) {
			Assert::That(Root().item, Is().Not().EqualTo(false));
		}
		Spec(with_int) {
			Assert::That(Root().item, Is().Not().EqualTo(5));
		}
		Spec(with_float) {
			Assert::That(Root().item, Is().Not().EqualTo(5.f));
		}
		Spec(with_double) {
			Assert::That(Root().item, Is().EqualTo(5.));
			Assert::That(Root().item, Is().Not().EqualTo(25.));
		}
		Spec(with_cstring) {
			Assert::That(Root().item, Is().Not().EqualTo("qwe"));
		}
		Spec(with_string) {
			Assert::That(Root().item, Is().Not().EqualTo(std::string {"qwe"}));
		}
		Spec(with_keystring) {
			Assert::That(Root().item, Is().Not().EqualTo(keystring {"qwe"}));
		}
	};
	Context(compare_string) {
		void SetUp() {
			Root().item = "asd";
		}
		Spec(with_bool) {
			Assert::That(Root().item, Is().Not().EqualTo(false));
		}
		Spec(with_int) {
			Assert::That(Root().item, Is().Not().EqualTo(5));
		}
		Spec(with_cstring) {
			Assert::That(Root().item, Is().Not().EqualTo("qwe"));
			Assert::That(Root().item, Is().EqualTo("asd"));
		}
		Spec(with_string) {
			Assert::That(Root().item, Is().Not().EqualTo(std::string {"qwe"}));
			Assert::That(Root().item, Is().EqualTo(std::string {"asd"}));
		}
		Spec(with_keystring) {
			Assert::That(Root().item, Is().Not().EqualTo(keystring {"qwe"}));
			Assert::That(Root().item, Is().EqualTo(keystring {"asd"}));
		}
	};
	Context(get_item_by_index) {
		property array_orig;
		property array_prop;
		ReflectedItem item{array_orig, array_prop};
		ConstReflectedItem citem{array_orig, array_prop};
		When(no_changes) {
			void SetUp() {
				Parent().array_orig[std::size_t(0)] = 1;
				Parent().array_orig[std::size_t(1)] = "qwe";
			}
			Then(size_is_two) {
				Assert::That(Parent().item.size(), Is().EqualTo(2U));
				Assert::That(Parent().citem.size(), Is().EqualTo(2U));
			}
			Then(access_by_zero_index_returns_int_1) {
				Assert::That(Parent().item.get_int_item(std::size_t(0)).as_int(), Is().EqualTo(1));
				Assert::That(Parent().citem.get_int_item(std::size_t(0)).as_int(), Is().EqualTo(1));
			}
			Then(access_by_first_index_returns_string_qwe) {
				Assert::That(Parent().item.get_string_item(std::size_t(1)).as_string(), Is().EqualTo("qwe"));
				Assert::That(Parent().citem.get_string_item(std::size_t(1)).as_string(), Is().EqualTo("qwe"));
			}
		};
		When(no_origin) {
			void SetUp() {
				Parent().array_prop[std::size_t(0)] = 1;
				Parent().array_prop[std::size_t(1)] = "qwe";
				Parent().array_prop[std::size_t(2)] = false;
			}
			Then(size_is_three) {
				Assert::That(Parent().item.size(), Is().EqualTo(3U));
				Assert::That(Parent().citem.size(), Is().EqualTo(3U));
			}
			Then(access_by_zero_index_returns_int_1) {
				Assert::That(Parent().item.get_int_item(std::size_t(0)).as_int(), Is().EqualTo(1));
				Assert::That(Parent().citem.get_int_item(std::size_t(0)).as_int(), Is().EqualTo(1));
			}
			Then(access_by_first_index_returns_string_qwe) {
				Assert::That(Parent().item.get_string_item(std::size_t(1)).as_string(), Is().EqualTo("qwe"));
				Assert::That(Parent().citem.get_string_item(std::size_t(1)).as_string(), Is().EqualTo("qwe"));
			}
			Then(access_by_second_index_returns_false) {
				Assert::That(Parent().item.get_bool_item(std::size_t(2)).as_bool(), Is().EqualTo(false));
				Assert::That(Parent().citem.get_bool_item(std::size_t(2)).as_bool(), Is().EqualTo(false));
			}
		};
		When(origin_and_changes_are_present) {
			void SetUp() {
				Parent().array_orig[std::size_t(0)] = 1;
				Parent().array_orig[std::size_t(1)] = "qwe";
				Parent().array_prop[std::size_t(0)] = 2;
				Parent().array_prop[std::size_t(1)] = "asd";
				Parent().array_prop[std::size_t(2)] = 15;
			}
			Then(size_is_three) {
				Assert::That(Parent().item.size(), Is().EqualTo(3U));
				Assert::That(Parent().citem.size(), Is().EqualTo(3U));
			}
			Then(access_by_zero_index_returns_int_2) {
				Assert::That(Parent().item.get_int_item(std::size_t(0)).as_int(), Is().EqualTo(2));
				Assert::That(Parent().citem.get_int_item(std::size_t(0)).as_int(), Is().EqualTo(2));
			}
			Then(access_by_first_index_returns_string_asd) {
				Assert::That(Parent().item.get_string_item(std::size_t(1)).as_string(), Is().EqualTo("asd"));
				Assert::That(Parent().citem.get_string_item(std::size_t(1)).as_string(), Is().EqualTo("asd"));
			}
			Then(access_by_second_index_returns_int_15) {
				Assert::That(Parent().item.get_int_item(std::size_t(2)).as_int(), Is().EqualTo(15));
				Assert::That(Parent().citem.get_int_item(std::size_t(2)).as_int(), Is().EqualTo(15));
			}
		};
	};
	Context(get_members) {
		When(origin_has_data_and_prop_is_dictionary) {
			void SetUp() {
				ReflectedItem edit{property::none_property(), Root().orig};
				edit.get_int_item("asd") = 15;
				edit.get_string_item("qwe") = "zxc";

				ReflectedItem edit2{property::none_property(), Root().diff};
				auto dict = edit2.get_dictionary_item("dict");
				dict.get_item("zxc") = "ZXC";
				dict.get_item("cxz") = "CXZ";
			}
			Then(check_diff) {
				property check;
				check["dict"]["zxc"] = "ZXC";
				check["dict"]["cxz"] = "CXZ";
				Assert::That(Root().diff, Is().EqualTo(check));
			}
			Then(check_members) {
				ReflectedItem edit{Root().orig, Root().diff};
				const auto& members = edit.get_dictionary_item("dict").get_members();
				Assert::That(members, Is().EqualTo(std::vector<keystring>{"cxz", "zxc"}));
			}
		};
		When(prop_is_none_and_orig_is_dict) {
			void SetUp() {
				ReflectedItem edit{property::none_property(), Root().orig};
				auto dict = edit.get_dictionary_item("dict");
				dict.get_item("zxc") = "ZXC";
				dict.get_item("cxz") = "CXZ";
			}
			Then(check_orig) {
				property check;
				check["dict"]["zxc"] = "ZXC";
				check["dict"]["cxz"] = "CXZ";
				Assert::That(Root().orig, Is().EqualTo(check));
			}
			Then(check_members) {
				ReflectedItem edit{Root().orig, Root().diff};
				const auto& members = edit.get_dictionary_item("dict").get_members();
				Assert::That(members, Is().EqualTo(std::vector<keystring>{"cxz", "zxc"}));
			}
		};
		When(prop_is_array_and_orig_is_dictionary) {
			void SetUp() {
				ReflectedItem edit{property::none_property(), Root().orig};
				auto dict = edit.get_dictionary_item("dict");
				dict.get_item("zxc") = "ZXC";
				dict.get_item("cxz") = "CXZ";

				ReflectedItem edit2{property::none_property(), Root().diff};
				auto dict2 = edit2.get_array_item("dict");
				dict2.get_item(0) = "RTY";
			}
			Then(check_orig) {
				property check;
				check["dict"]["zxc"] = "ZXC";
				check["dict"]["cxz"] = "CXZ";
				Assert::That(Root().orig, Is().EqualTo(check));
			}
			Then(check_diff) {
				property check;
				check["dict"][std::size_t(0)] = "RTY";
				Assert::That(Root().diff, Is().EqualTo(check));
			}
			Then(check_members) {
				ReflectedItem edit{Root().orig, Root().diff};
				AssertThrowsEx(UnexpectedItemTypeException, edit.get_item("dict").get_members());
			}
		};
		When(prop_is_none_and_orig_is_array) {
			void SetUp() {
				ReflectedItem edit{property::none_property(), Root().orig};
				auto dict = edit.get_dictionary_item("dict");
				dict.get_item(std::size_t(0)) = "ZXC";
				dict.get_item(std::size_t(1)) = "CXZ";
			}
			Then(check_orig) {
				property check;
				check["dict"][std::size_t(0)] = "ZXC";
				check["dict"][std::size_t(1)] = "CXZ";
				Assert::That(Root().orig, Is().EqualTo(check));
			}
			Then(check_diff) {
				property check;
				Assert::That(Root().diff, Is().EqualTo(check));
			}
			Then(check_members) {
				ReflectedItem edit{Root().orig, Root().diff};
				AssertThrowsEx(UnexpectedItemTypeException, edit.get_item("dict").get_members());
			}
		};
		When(prop_is_dictionary_and_orig_is_dictionary) {
			void SetUp() {
				ReflectedItem edit{property::none_property(), Root().orig};
				auto dict = edit.get_dictionary_item("dict");
				dict.get_item("zxc") = "ZXC";
				dict.get_item("cxz") = "CXZ";

				ReflectedItem edit2{property::none_property(), Root().diff};
				auto dict2 = edit2.get_dictionary_item("dict");
				dict2.get_item("rty") = "RTY";
			}
			Then(check_orig) {
				property check;
				check["dict"]["zxc"] = "ZXC";
				check["dict"]["cxz"] = "CXZ";
				Assert::That(Root().orig, Is().EqualTo(check));
			}
			Then(check_diff) {
				property check;
				check["dict"]["rty"] = "RTY";
				Assert::That(Root().diff, Is().EqualTo(check));
			}
			Then(check_members) {
				ReflectedItem edit{Root().orig, Root().diff};
				const auto& members = edit.get_dictionary_item("dict").get_members();
				Assert::That(members, Is().EqualTo(std::vector<keystring>{"rty"}));
			}
		};
	};
};

Describe(model_path) {
	Context(add_path) {
		When(add_empty_to_empty) {
			keystring result;
			void SetUp() {
				result = path::add("", "");
			}
			Then(result_is_empty) {
				Assert::That(result.c_str(), Is().EqualTo(""));
			}
		};
		When(add_slash_to_slash) {
			keystring result;
			void SetUp() {
				result = path::add("/", "/");
			}
			Then(result_is_empty) {
				Assert::That(result.c_str(), Is().EqualTo(""));
			}
		};
		When(add_slash_slash_to_slash_slash) {
			keystring result;
			void SetUp() {
				result = path::add("//", "//");
			}
			Then(result_is_empty) {
				Assert::That(result.c_str(), Is().EqualTo(""));
			}
		};
		When(add_backslash_to_backslash) {
			keystring result;
			void SetUp() {
				result = path::add("\\", "\\");
			}
			Then(result_is_empty) {
				Assert::That(result.c_str(), Is().EqualTo("\\/\\"));
			}
		};
		When(add_empty_to_parent) {
			keystring result;
			void SetUp() {
				result = path::add("parent", "");
			}
			Then(result_is_parent) {
				Assert::That(result.c_str(), Is().EqualTo("parent"));
			}
		};
		When(add_child_to_empty) {
			keystring result;
			void SetUp() {
				result = path::add("", "child");
			}
			Then(result_is_child) {
				Assert::That(result.c_str(), Is().EqualTo("child"));
			}
		};
		When(add_child_to_parent) {
			keystring result;
			void SetUp() {
				result = path::add("parent", "child");
			}
			Then(result_is_child) {
				Assert::That(result.c_str(), Is().EqualTo("parent/child"));
			}
		};
		When(add_slash_child_slash_to_slash_parent_slash) {
			keystring result;
			void SetUp() {
				result = path::add("/parent/", "/child/");
			}
			Then(result_is_child) {
				Assert::That(result.c_str(), Is().EqualTo("parent/child"));
			}
		};
		When(add_slash_slash_child_slash_slash_to_slash_slash_parent_slash_slash) {
			keystring result;
			void SetUp() {
				result = path::add("//parent//", "//child//");
			}
			Then(result_is_child) {
				Assert::That(result.c_str(), Is().EqualTo("parent/child"));
			}
		};
		When(add_child_to_family) {
			keystring result;
			void SetUp() {
				result = path::add("grandpa/parent", "child");
			}
			Then(result_is_child) {
				Assert::That(result.c_str(), Is().EqualTo("grandpa/parent/child"));
			}
		};
		When(add_family_to_grandpa) {
			keystring result;
			void SetUp() {
				result = path::add("grandma", "parent/child");
			}
			Then(result_is_child) {
				Assert::That(result.c_str(), Is().EqualTo("grandma/parent/child"));
			}
		};
		When(add_family_to_family) {
			keystring result;
			void SetUp() {
				result = path::add("grandma/parent", "child/grandson");
			}
			Then(result_is_child) {
				Assert::That(result.c_str(), Is().EqualTo("grandma/parent/child/grandson"));
			}
		};
	};
	Context(front) {
		When(front_from_empty) {
			keystring front;
			keystring tail;
			void SetUp() {
				front = path::front("", tail);
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
			keystring tail;
			void SetUp() {
				front = path::front("child", tail);
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
			keystring tail;
			void SetUp() {
				front = path::front("parent/child", tail);
			}
			Then(front_is_parent) {
				Assert::That(front.c_str(), Is().EqualTo("parent"));
			}
			Then(tail_is_child) {
				Assert::That(tail.c_str(), Is().EqualTo("child"));
			}
		};
		When(front_from_slash_slash_child) {
			keystring front;
			keystring tail;
			void SetUp() {
				front = path::front("//child", tail);
			}
			Then(front_is_child) {
				Assert::That(front.c_str(), Is().EqualTo("child"));
			}
			Then(tail_is_empty) {
				Assert::That(tail.empty(), Is().EqualTo(true));
			}
		};
		When(front_from_parent_slash_slash) {
			keystring front;
			keystring tail;
			void SetUp() {
				front = path::front("parent//", tail);
			}
			Then(front_is_parent) {
				Assert::That(front.c_str(), Is().EqualTo("parent"));
			}
			Then(tail_is_empty) {
				Assert::That(tail.empty(), Is().EqualTo(true));
			}
		};
		When(front_from_slash) {
			keystring front;
			keystring tail;
			void SetUp() {
				front = path::front("/", tail);
			}
			Then(front_is_empty) {
				Assert::That(front.empty(), Is().EqualTo(true));
			}
			Then(tail_is_empty) {
				Assert::That(tail.empty(), Is().EqualTo(true));
			}
		};
		When(front_from_parent_slash_child_slash_grandson) {
			keystring front;
			keystring tail;
			void SetUp() {
				front = path::front("parent/child/grandson", tail);
			}
			Then(front_is_parent) {
				Assert::That(front.c_str(), Is().EqualTo("parent"));
			}
			Then(tail_is_child_grandson) {
				Assert::That(tail.c_str(), Is().EqualTo("child/grandson"));
			}
		};
		When(front_from_parent_slash_child_slash_slash_grandson) {
			keystring front;
			keystring tail;
			void SetUp() {
				front = path::front("parent/child//grandson", tail);
			}
			Then(front_is_parent) {
				Assert::That(front.c_str(), Is().EqualTo("parent"));
			}
			Then(tail_is_child_grandson) {
				Assert::That(tail.c_str(), Is().EqualTo("child//grandson"));
			}
		};
	};
	Context(back) {
		When(back_from_empty) {
			keystring back;
			keystring head;
			void SetUp() {
				back = path::back("", head);
			}
			Then(back_is_empty) {
				Assert::That(back.empty(), Is().EqualTo(true));
			}
			Then(head_is_empty) {
				Assert::That(head.empty(), Is().EqualTo(true));
			}
		};
		When(back_from_child) {
			keystring back;
			keystring head;
			void SetUp() {
				back = path::back("child", head);
			}
			Then(back_is_child) {
				Assert::That(back.c_str(), Is().EqualTo("child"));
			}
			Then(head_is_empty) {
				Assert::That(head.empty(), Is().EqualTo(true));
			}
		};
		When(back_from_parent_slash_child) {
			keystring back;
			keystring head;
			void SetUp() {
				back = path::back("parent/child", head);
			}
			Then(back_is_child) {
				Assert::That(back.c_str(), Is().EqualTo("child"));
			}
			Then(head_is_parent) {
				Assert::That(head.c_str(), Is().EqualTo("parent"));
			}
		};
		When(back_from_slash_slash_child) {
			keystring back;
			keystring head;
			void SetUp() {
				back = path::back("//child", head);
			}
			Then(back_is_child) {
				Assert::That(back.c_str(), Is().EqualTo("child"));
			}
			Then(head_is_empty) {
				Assert::That(head.empty(), Is().EqualTo(true));
			}
		};
		When(back_from_parent_slash_slash) {
			keystring back;
			keystring head;
			void SetUp() {
				back = path::back("parent//", head);
			}
			Then(back_is_parent) {
				Assert::That(back.c_str(), Is().EqualTo("parent"));
			}
			Then(head_is_empty) {
				Assert::That(head.empty(), Is().EqualTo(true));
			}
		};
		When(back_from_slash) {
			keystring back;
			keystring head;
			void SetUp() {
				back = path::back("/", head);
			}
			Then(back_is_child) {
				Assert::That(back.empty(), Is().EqualTo(true));
			}
			Then(head_is_empty) {
				Assert::That(head.empty(), Is().EqualTo(true));
			}
		};
		When(back_from_parent_slash_child_slash_grandson) {
			keystring back;
			keystring head;
			void SetUp() {
				back = path::back("parent/child/grandson", head);
			}
			Then(back_is_grandson) {
				Assert::That(back.c_str(), Is().EqualTo("grandson"));
			}
			Then(head_is_parent_child) {
				Assert::That(head.c_str(), Is().EqualTo("parent/child"));
			}
		};
		When(back_from_parent_slash_child_slash_slash_grandson) {
			keystring back;
			keystring head;
			void SetUp() {
				back = path::back("parent/child//grandson", head);
			}
			Then(back_is_child) {
				Assert::That(back.c_str(), Is().EqualTo("grandson"));
			}
			Then(head_is_parent_child) {
				Assert::That(head.c_str(), Is().EqualTo("parent/child"));
			}
		};
	};
};

