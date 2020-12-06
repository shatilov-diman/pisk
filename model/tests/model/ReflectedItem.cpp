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

#include <pisk/model/ReflectedItem.h>
#include <pisk/model/ReflectedObject.h>

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
	Spec(item_is_not_changed) {
		Assert::That(item.is_changed(), Is().EqualTo(false));
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
		Then(item_is_changed) {
			Assert::That(Root().item.is_changed(), Is().EqualTo(true));
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
			Assert::That(Root().item.is_changed(), Is().EqualTo(true));
			AssertThrowsEx(PropertyCastException, Root().item.size());
		}
	};
	When(property_init_with_int) {
		Then(item_is_int) {
			Root().diff = 7;
			Assert::That(Root().item.is_int(), Is().EqualTo(true));
			Assert::That(Root().item.as_int(), Is().EqualTo(7));
			Assert::That(Root().item.is_changed(), Is().EqualTo(true));
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
			Root().diff["7"][std::size_t(2)] = 42;
			Root().diff["8"] = 146L;
			Root().diff["9"] = 732.f;
			Root().diff["a"] = 5.;
		}
		Then(item_is_dictionary) {
			Assert::That(Root().item.is_dictionary(), Is().EqualTo(true));
			Assert::That(citem.is_dictionary(), Is().EqualTo(true));
			Assert::That(Root().item.size(), Is().EqualTo(9U));
		}
		Spec(get_bool_item) {
			Assert::That(Root().item.get_bool_item("2").as_bool(), Is().EqualTo(true));
			Assert::That(citem.get_bool_item("2").as_bool(), Is().EqualTo(true));
		}
		Spec(get_int_item) {
			Assert::That(Root().item.get_int_item("3").as_int(), Is().EqualTo(15));
			Assert::That(citem.get_int_item("3").as_int(), Is().EqualTo(15));
		}
		Spec(get_long_item) {
			Assert::That(Root().item.get_long_item("8").as_long(), Is().EqualTo(146L));
			Assert::That(citem.get_long_item("8").as_long(), Is().EqualTo(146L));
		}
		Spec(get_float_item) {
			Assert::That(Root().item.get_float_item("9").as_float(), Is().EqualTo(732.f));
			Assert::That(citem.get_float_item("9").as_float(), Is().EqualTo(732.f));
		}
		Spec(get_double_item) {
			Assert::That(Root().item.get_double_item("a").as_double(), Is().EqualTo(5.));
			Assert::That(citem.get_double_item("a").as_double(), Is().EqualTo(5.));
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
		Spec(get_custom_item_array) {
			Assert::That(Root().item.get_custom_item<ReflectedItem>("3").is_int(), Is().EqualTo(true));
			Assert::That(Root().item.get_custom_item<ReflectedItem>("3").as_int(), Is().EqualTo(15));
			Assert::That(citem.get_custom_item<ConstReflectedItem>("3").is_int(), Is().EqualTo(true));
			Assert::That(citem.get_custom_item<ConstReflectedItem>("3").as_int(), Is().EqualTo(15));
		}
		Spec(get_custom_item_index) {
			Assert::That(Root().item.get_item("7").get_custom_item<ReflectedItem>(2).is_int(), Is().EqualTo(true));
			Assert::That(Root().item.get_item("7").get_custom_item<ReflectedItem>(2).as_int(), Is().EqualTo(42));
			Assert::That(citem.get_item("7").get_custom_item<ConstReflectedItem>(2).is_int(), Is().EqualTo(true));
			Assert::That(citem.get_item("7").get_custom_item<ConstReflectedItem>(2).as_int(), Is().EqualTo(42));
		}
		Spec(get_incorrect_item_type_int) {
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("2"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_bool_item("3"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("4"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("5"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("7"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("8"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("9"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_int_item("a"));
		}
		Spec(get_incorrect_item_type_long) {
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_long_item("2"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_long_item("3"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_long_item("4"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_long_item("5"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_long_item("7"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_bool_item("8"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_long_item("9"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_long_item("a"));
		}
		Spec(get_incorrect_item_type_float) {
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_float_item("2"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_float_item("3"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_float_item("4"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_float_item("5"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_float_item("7"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_float_item("8"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_bool_item("9"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_float_item("a"));
		}
		Spec(get_incorrect_item_type_double) {
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_double_item("2"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_double_item("3"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_double_item("4"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_double_item("5"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_double_item("7"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_double_item("8"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_double_item("9"));
			AssertThrowsEx(UnexpectedItemTypeException, Root().item.get_bool_item("a"));
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
		Spec(get_long_for_none) {
			Assert::That(Root().item.get_long_item("!2").is_none(), Is().EqualTo(true));
			Root().item.get_long_item("!2") = 17L;
			Assert::That(Root().item.get_long_item("!2").as_long(), Is().EqualTo(17L));
		}
		Spec(get_float_for_none) {
			Assert::That(Root().item.get_float_item("!2").is_none(), Is().EqualTo(true));
			Root().item.get_float_item("!2") = 17.F;
			Assert::That(Root().item.get_float_item("!2").as_float(), Is().EqualTo(17.F));
		}
		Spec(get_double_for_none) {
			Assert::That(Root().item.get_double_item("!2").is_none(), Is().EqualTo(true));
			Root().item.get_double_item("!2") = 17.;
			Assert::That(Root().item.get_double_item("!2").as_double(), Is().EqualTo(17.));
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
			Assert::That(Root().item.is_changed(), Is().EqualTo(true));
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
	Context(remove_members) {
		When(remove_item_from_empty) {
			Then(expected_item_type_will_thrown) {
				ReflectedItem root{Root().orig, Root().diff};
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item(7U));
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item("asd"));
			}
		};
		When(remove_item_from_int) {
			void SetUp() {
				Root().orig = 42;
			}
			Then(expected_item_type_will_thrown) {
				ReflectedItem root{Root().orig, Root().diff};
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item(7U));
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item("asd"));
			}
		};
		When(remove_item_from_string) {
			void SetUp() {
				Root().orig = "asd";
			}
			Then(expected_item_type_will_thrown) {
				ReflectedItem root{Root().orig, Root().diff};
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item(7U));
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item("asd"));
			}
		};
		When(remove_item_from_double) {
			void SetUp() {
				Root().orig = 7.;
			}
			Then(expected_item_type_will_thrown) {
				ReflectedItem root{Root().orig, Root().diff};
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item(7U));
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item("asd"));
			}
		};
		When(remove_item_from_int_when_orig_is_array) {
			void SetUp() {
				Root().orig = property::array{{7U, "test"}};
				Root().diff = 41;
			}
			Then(expected_item_type_will_thrown) {
				ReflectedItem root{Root().orig, Root().diff};
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item(7U));
			}
		};
		When(remove_item_from_string_when_orig_is_dictionary) {
			void SetUp() {
				Root().orig = property::dictionary{{"key", "test"}};
				Root().diff = "asd";
			}
			Then(expected_item_type_will_thrown) {
				ReflectedItem root{Root().orig, Root().diff};
				AssertThrowsEx(UnexpectedItemTypeException, root.remove_item("key"));
			}
		};
		When(remove_item_from_array) {
			void SetUp() {
				Root().orig = property::array{{0U, "test"}, {1U, 777.}, {2U, property{} }};
			}
			When(middle_item_removed) {
				void SetUp() {
					ReflectedItem root{Root().orig, Root().diff};
					root.remove_item(1U);
				}
				Then(orig_is_not_changed) {
					property check;
					check[std::size_t(0)] = "test";
					check[std::size_t(1)] = 777.;
					check[std::size_t(2)] = property{};
					Assert::That(Root().orig, Is().EqualTo(check));
				}
				Then(diff_is_chagged) {
					property check;
					check[std::size_t(0)] = "test";
					check[std::size_t(1)] = property{};
					Assert::That(Root().diff, Is().EqualTo(check));
				}
				When(last_item_removed) {
					void SetUp() {
						ReflectedItem root{Root().orig, Root().diff};
						root.remove_item(1U);
					}
					Then(orig_is_not_changed) {
						property check;
						check[std::size_t(0)] = "test";
						check[std::size_t(1)] = 777.;
						check[std::size_t(2)] = property{};
						Assert::That(Root().orig, Is().EqualTo(check));
					}
					Then(diff_is_chagged) {
						property check;
						check[std::size_t(0)] = "test";
						Assert::That(Root().diff, Is().EqualTo(check));
					}
					When(first_item_removed) {
						void SetUp() {
							ReflectedItem root{Root().orig, Root().diff};
							root.remove_item(0U);
						}
						Then(orig_is_not_changed) {
							property check;
							check[std::size_t(0)] = "test";
							check[std::size_t(1)] = 777.;
							check[std::size_t(2)] = property{};
							Assert::That(Root().orig, Is().EqualTo(check));
						}
						Then(diff_is_chagged) {
							property check{property::array{}};
							Assert::That(Root().diff, Is().EqualTo(check));
						}
					};
				};
			};
		};
		When(remove_item_from_dictionary) {
			void SetUp() {
				Root().orig = property::dictionary{{"aaa", "test"}, {"bbb", 777.}, {"ccc", property{} }};
			}
			When(middle_item_removed) {
				void SetUp() {
					ReflectedItem root{Root().orig, Root().diff};
					root.remove_item("bbb");
				}
				Then(orig_is_not_changed) {
					property check;
					check["aaa"] = "test";
					check["bbb"] = 777.;
					check["ccc"] = property{};
					Assert::That(Root().orig, Is().EqualTo(check));
				}
				Then(diff_is_chagged) {
					property check;
					check["aaa"] = "test";
					check["ccc"] = property{};
					Assert::That(Root().diff, Is().EqualTo(check));
				}
				When(last_item_removed) {
					void SetUp() {
						ReflectedItem root{Root().orig, Root().diff};
						root.remove_item("ccc");
					}
					Then(orig_is_not_changed) {
						property check;
						check["aaa"] = "test";
						check["bbb"] = 777.;
						check["ccc"] = property{};
						Assert::That(Root().orig, Is().EqualTo(check));
					}
					Then(diff_is_chagged) {
						property check;
						check["aaa"] = "test";
						Assert::That(Root().diff, Is().EqualTo(check));
					}
					When(first_item_removed) {
						void SetUp() {
							ReflectedItem root{Root().orig, Root().diff};
							root.remove_item("aaa");
						}
						Then(orig_is_not_changed) {
							property check;
							check["aaa"] = "test";
							check["bbb"] = 777.;
							check["ccc"] = property{};
							Assert::That(Root().orig, Is().EqualTo(check));
						}
						Then(diff_is_chagged) {
							property check{property::dictionary{}};
							Assert::That(Root().diff, Is().EqualTo(check));
						}
					};
				};
			};
		};
	};
	Context(casting) {
		void SetUp() {
			ReflectedObject obj{property::none_property(), Root().diff};
			obj.id() = "123";
			obj.child("321");
		}
		Then(check_diff) {
			property check;
			check["properties"]["id"] = "123";
			check["children"]["321"]["properties"]["id"] = "321";
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		Then(check_cast) {
			ReflectedItem root{Root().orig, Root().diff};
			const auto& obj = root.cast<ReflectedObject>();
			Assert::That(obj.id().as_string(), Is().EqualTo("123"));
			Assert::That(obj.child("321").id().as_string(), Is().EqualTo("321"));
		}
		Then(check_const_cast) {
			ConstReflectedItem root{Root().orig, Root().diff};
			const auto& obj = root.cast<ConstReflectedObject>();
			Assert::That(obj.id().as_string(), Is().EqualTo("123"));
			Assert::That(obj.child("321").id().as_string(), Is().EqualTo("321"));
		}
	};
	Context(is_changed) {
		When(item_is_none) {
			Then(is_not_changed) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(false));
				Assert::That(Root().item.has_changes(), Is().EqualTo(false));
				Assert::That(Root().item.is_changed(), Is().EqualTo(false));
			}
		};
		When(item_is_changed) {
			void SetUp() {
				Root().diff = 1;
			}
			Then(is_changed) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(false));
				Assert::That(Root().item.has_changes(), Is().EqualTo(true));
				Assert::That(Root().item.is_changed(), Is().EqualTo(true));
			}
		};
		When(item_is_not_changed) {
			void SetUp() {
				Root().orig = 1;
			}
			Then(is_not_changed) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(true));
				Assert::That(Root().item.has_changes(), Is().EqualTo(false));
				Assert::That(Root().item.is_changed(), Is().EqualTo(true));
			}
		};
		When(item_has_changes_and_origin_same_type) {
			void SetUp() {
				Root().orig = 1;
				Root().diff = 2;
			}
			Then(is_not_changed) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(true));
				Assert::That(Root().item.has_changes(), Is().EqualTo(true));
				Assert::That(Root().item.is_changed(), Is().EqualTo(true));
			}
		};
		When(item_has_changes_and_origin_diff_type) {
			void SetUp() {
				Root().orig = 1;
				Root().diff = "string";
			}
			Then(is_not_changed) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(true));
				Assert::That(Root().item.has_changes(), Is().EqualTo(true));
				Assert::That(Root().item.is_changed(), Is().EqualTo(true));
			}
		};
		When(item_has_changes_and_origin_as_int_with_same_value) {
			void SetUp() {
				Root().orig = 1;
				Root().diff = 1;
			}
			Then(is_not_changed) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(true));
				Assert::That(Root().item.has_changes(), Is().EqualTo(true));
				Assert::That(Root().item.is_changed(), Is().EqualTo(false));
			}
		};
		When(item_has_changes_and_origin_as_string_with_same_value) {
			void SetUp() {
				Root().orig = "qwe";
				Root().diff = "qwe";
			}
			Then(is_not_changed) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(true));
				Assert::That(Root().item.has_changes(), Is().EqualTo(true));
				Assert::That(Root().item.is_changed(), Is().EqualTo(false));
			}
		};
		When(item_has_changes_and_origin_as_array_with_same_value) {
			void SetUp() {
				Root().orig[std::size_t(0)] = "qwe";
				Root().diff[std::size_t(0)] = "qwe";
			}
			Then(is_not_changed) {
				Assert::That(Root().item.has_origin(), Is().EqualTo(true));
				Assert::That(Root().item.has_changes(), Is().EqualTo(true));
				Assert::That(Root().item.is_changed(), Is().EqualTo(false));
			}
		};
	};
};

