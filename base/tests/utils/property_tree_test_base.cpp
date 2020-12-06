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

Describe(property_tree) {
	It(_init_default) {
		property prop;
		Assert::That(prop.is_none(), Is().EqualTo(true));
		Assert::That(prop.is_bool(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_none));
	}
	It(_init_nullptr) {
		property prop(nullptr);
		Assert::That(prop.is_none(), Is().EqualTo(true));
		Assert::That(prop.is_bool(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_none));
	}
	It(_init_with_bool) {
		property prop(true);
		Assert::That(prop.is_bool(), Is().EqualTo(true));
		Assert::That(prop.is_none(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_bool));
		Assert::That(static_cast<bool>(prop), Is().EqualTo(true));
		AssertThrowsEx(PropertyCastException, static_cast<int>(prop));
	}
	It(_init_with_int) {
		property prop(5);
		Assert::That(prop.is_int(), Is().EqualTo(true));
		Assert::That(prop.is_long(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_int));
		Assert::That(static_cast<int>(prop), Is().EqualTo(5));
		AssertThrowsEx(PropertyCastException, static_cast<long>(prop));
	}
	It(_init_with_long) {
		property prop(5L);
		Assert::That(prop.is_long(), Is().EqualTo(true));
		Assert::That(prop.is_int(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_long));
		Assert::That(static_cast<long>(prop), Is().EqualTo(5));
		AssertThrowsEx(PropertyCastException, static_cast<int>(prop));
	}
	It(_init_with_float) {
		property prop(5.f);
		Assert::That(prop.is_float(), Is().EqualTo(true));
		Assert::That(prop.is_double(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_float));
		Assert::That(static_cast<float>(prop), Is().EqualTo(5.f));
		AssertThrowsEx(PropertyCastException, static_cast<double>(prop));
	}
	It(_init_with_double) {
		property prop(5.);
		Assert::That(prop.is_double(), Is().EqualTo(true));
		Assert::That(prop.is_float(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_double));
		Assert::That(static_cast<double>(prop), Is().EqualTo(5.));
		AssertThrowsEx(PropertyCastException, static_cast<float>(prop));
	}
	static void check_string_type(const property& prop) {
		Assert::That(prop.is_string(), Is().EqualTo(true));
		Assert::That(prop.is_dictionary(), Is().EqualTo(false));
		Assert::That(prop.is_array(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_string));
	}
	It(_init_with_c_string) {
		property prop("hello");
		check_string_type(prop);
		Assert::That(static_cast<std::string>(prop), Is().EqualTo("hello"));
		Assert::That(static_cast<keystring>(prop), Is().EqualTo(keystring("hello")));
		AssertThrowsEx(PropertyCastException, static_cast<double>(prop));
	}
	It(_init_with_std_string) {
		property prop(std::string("hello"));
		check_string_type(prop);
		Assert::That(static_cast<std::string>(prop), Is().EqualTo("hello"));
		Assert::That(static_cast<keystring>(prop), Is().EqualTo(keystring("hello")));
		AssertThrowsEx(PropertyCastException, static_cast<double>(prop));
	}
	It(_init_with_keystring) {
		property prop(keystring("hello"));
		check_string_type(prop);
		Assert::That(static_cast<std::string>(prop), Is().EqualTo("hello"));
		Assert::That(static_cast<keystring>(prop), Is().EqualTo(keystring("hello")));
		AssertThrowsEx(PropertyCastException, static_cast<double>(prop));
	}
	It(_init_with_dictionary) {
		property prop(property::dictionary {});
		Assert::That(prop.is_dictionary(), Is().EqualTo(true));
		Assert::That(prop.is_string(), Is().EqualTo(false));
		Assert::That(prop.is_array(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_dictionary));
		Assert::That(prop["key"].get_type(), Is().EqualTo(property::type::_none));
		AssertThrowsEx(PropertyCastException, static_cast<std::string>(prop));
		AssertThrowsEx(PropertyCastException, static_cast<std::string>(prop["key"]));
		prop["key"] = 15;
		Assert::That(prop["key"].get_type(), Is().EqualTo(property::type::_int));
		Assert::That(static_cast<int>(prop["key"]), Is().EqualTo(15));
		AssertThrowsEx(PropertyCastException, static_cast<float>(prop["key"]));
	}
	It(_init_with_array) {
		property prop(property::array {});
		Assert::That(prop.is_array(), Is().EqualTo(true));
		Assert::That(prop.is_string(), Is().EqualTo(false));
		Assert::That(prop.is_dictionary(), Is().EqualTo(false));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_array));
		Assert::That(prop[std::size_t()].get_type(), Is().EqualTo(property::type::_none));
		AssertThrowsEx(PropertyCastException, static_cast<std::string>(prop));
		AssertThrowsEx(PropertyCastException, static_cast<std::string>(prop[std::size_t()]));
		prop[1] = 15.f;
		Assert::That(prop[1].get_type(), Is().EqualTo(property::type::_float));
		Assert::That(static_cast<float>(prop[1]), Is().EqualTo(15.f));
		AssertThrowsEx(PropertyCastException, static_cast<int>(prop[1]));
		prop[2] = "hehehe";
	}
	It(_uses_as_property_tree) {
		property prop(property::dictionary {});
		prop["phys"]["speed"] = 23;
		prop["phys"]["accelerate"] = 23.5;
		prop["name"] = "object_1";
		Assert::That(prop["phys"].get_type(), Is().EqualTo(property::type::_dictionary));
		Assert::That(prop["phys"]["speed"].get_type(), Is().EqualTo(property::type::_int));
		Assert::That(static_cast<int>(prop["phys"]["speed"]), Is().EqualTo(23));
		Assert::That(prop["phys"]["accelerate"].get_type(), Is().EqualTo(property::type::_double));
		Assert::That(static_cast<double>(prop["phys"]["accelerate"]), Is().EqualTo(23.5));
		Assert::That(prop["name"].get_type(), Is().EqualTo(property::type::_string));
		Assert::That(static_cast<std::string>(prop["name"]), Is().EqualTo("object_1"));
	}
	It(_set_string_to_string) {
		property prop(keystring("hello"));
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_string));
		Assert::That(static_cast<std::string>(prop), Is().EqualTo("hello"));

		prop = keystring("buy");
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_string));
		Assert::That(static_cast<std::string>(prop), Is().EqualTo("buy"));
	}
	It(_set_property_tree_to_property_tree) {
		property prop(property::dictionary {});
		prop["a"] = "b";
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_dictionary));
		Assert::That(prop["a"].get_type(), Is().EqualTo(property::type::_string));
		Assert::That(static_cast<std::string>(prop["a"]), Is().EqualTo("b"));

		property prop2;
		prop2["x"] = "y";
		prop = static_cast<property::dictionary>(prop2);

		Assert::That(prop.get_type(), Is().EqualTo(property::type::_dictionary));
		Assert::That(prop["x"].get_type(), Is().EqualTo(property::type::_string));
		Assert::That(static_cast<std::string>(prop["x"]), Is().EqualTo("y"));
	}
	It(_set_property_array_to_property_array) {
		property prop(property::array {});
		prop[std::size_t(0)] = "a";
		prop[1] = 1;
		Assert::That(prop.get_type(), Is().EqualTo(property::type::_array));
		Assert::That(prop[std::size_t(0)].get_type(), Is().EqualTo(property::type::_string));
		Assert::That(prop[1].get_type(), Is().EqualTo(property::type::_int));
		Assert::That(static_cast<std::string>(prop[std::size_t(0)]), Is().EqualTo("a"));
		Assert::That(static_cast<int>(prop[1]), Is().EqualTo(1));

		property prop2;
		prop2[std::size_t(0)] = "x";
		prop2[1] = 1.2f;
		prop = static_cast<property::array>(prop2);

		Assert::That(prop.get_type(), Is().EqualTo(property::type::_array));
		Assert::That(prop[std::size_t(0)].get_type(), Is().EqualTo(property::type::_string));
		Assert::That(prop[1].get_type(), Is().EqualTo(property::type::_float));
		Assert::That(static_cast<std::string>(prop[std::size_t(0)]), Is().EqualTo("x"));
		Assert::That(static_cast<float>(prop[1]), Is().EqualTo(1.2f));
	}
	It(_compare_string) {
		property prop(keystring("hello"));
		property prop2(keystring("hello"));
		Assert::That(prop, Is().EqualTo(prop2));
	}
	It(_compare_dictionary) {
		property prop;
		prop["a"] = "a";
		property prop2;
		prop2["a"] = "a";
		Assert::That(prop, Is().EqualTo(prop2));
	}
	It(_compare_array) {
		property prop;
		prop[std::size_t(0)] = "a";
		prop[1] = 1;

		property prop2;
		prop2[std::size_t(0)] = "a";
		prop2[1] = 1;

		Assert::That(prop, Is().EqualTo(prop2));
	}
	It(_compare_string_and_int) {
		property prop(keystring("hello"));
		property prop2(1);
		Assert::That(prop, Is().Not().EqualTo(prop2));
	}
	It(_compare_int_and_none) {
		property prop;
		property prop2(1);
		Assert::That(prop, Is().Not().EqualTo(prop2));
	}
	It(_compare_diff_int_and_float) {
		property prop(3);
		property prop2(2.f);
		Assert::That(prop, Is().Not().EqualTo(prop2));
	}
	It(_compare_diff_int_and_float_with_equal_value) {
		property prop(3);
		property prop2(3.f);
		Assert::That(prop, Is().Not().EqualTo(prop2));
	}
	It(_compare_diff_double_and_float) {
		property prop(3.);
		property prop2(2.f);
		Assert::That(prop, Is().Not().EqualTo(prop2));
	}
	It(_compare_diff_double_and_float_with_equal_value) {
		property prop(3.);
		property prop2(3.f);
		Assert::That(prop, Is().Not().EqualTo(prop2));
	}
	It(_compare_diff_int) {
		property prop(3);
		property prop2(1);
		Assert::That(prop, Is().Not().EqualTo(prop2));
	}
	It(_compare_diff_string) {
		property prop("hello");
		property prop2(keystring("hello2"));
		Assert::That(prop, Is().Not().EqualTo(prop2));
	}
	It(_compare_double_and_double_with_equal_value) {
		property prop(3.);
		property prop2(3.);
		Assert::That(prop, Is().EqualTo(prop2));
	}
	Context(_compare_iterators) {
		Spec(check_iteratable) {
			property prop;
			prop["a"] = "c";
			prop["b"] = 1;
			const property& prop2 = prop;
			AssertThrowsEx(PropertyCastException, prop["a"].begin());
			AssertThrowsEx(PropertyCastException, prop["b"].end());
			AssertThrowsEx(PropertyCastException, prop2["a"].begin());
			AssertThrowsEx(PropertyCastException, prop2["b"].end());
		}
		Spec(compare_in_one_container) {
			property prop;
			prop["a"] = "c";
			prop["b"] = "d";
			const property& prop2 = prop;
			Assert::That(prop.begin(), Is().EqualTo(prop.begin()));
			Assert::That(prop.end(), Is().EqualTo(prop.end()));
			Assert::That(prop.begin(), Is().Not().EqualTo(prop.end()));
			Assert::That(prop.begin(), Is().EqualTo(prop2.begin()));
			Assert::That(prop.begin(), Is().Not().EqualTo(++prop.begin()));
			Assert::That(prop.begin(), Is().EqualTo(prop.begin()++));
			Assert::That(++prop.begin(), Is().EqualTo(++prop2.begin()));
		}
		Spec(enumerate_none) {
			property prop(property::dictionary {
				std::make_pair(keystring("a"), property("b")),
			});
			const property prop2 = prop;
			for (const auto& x : prop["some_non_exists_key"]["subkey"])
			{
				UNUSED(x);
				Assert::Failure("WTF?");
			}
			for (const auto& x : prop2["some_non_exists_key"]["subkey"])
			{
				UNUSED(x);
				Assert::Failure("WTF?");
			}
			Assert::That(prop["another_non_exists_key"].is_none(), Is().EqualTo(true));
			Assert::That(prop["another_non_exists_key"].get_type(), Is().EqualTo(property::type::_none));
			AssertThrowsEx(PropertyCastException, prop["a"].begin());
		}
		Spec(iterators) {
			property prop(property::dictionary {
				std::make_pair(keystring("a"), property("b")),
				std::make_pair(keystring("c"), property("d")),
			});
			Assert::That(prop.begin().get_key().get_content(), Is().EqualTo("a"));
			Assert::That(prop.begin()->as_string(), Is().EqualTo("b"));
			Assert::That((++prop.begin()).get_key().get_content(), Is().EqualTo("c"));
			Assert::That((++prop.begin())->as_string(), Is().EqualTo("d"));
			Assert::That((prop.begin()++)->as_string(), Is().EqualTo("b"));
			Assert::That((++(++prop.begin())), Is().EqualTo(prop.end()));
		}
	};
};

