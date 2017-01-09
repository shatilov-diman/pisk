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
		Spec(compare_in_two_containers) {
			property prop(property::dictionary {
				std::make_pair(keystring("a"), property("b")),
			});
			const property prop2(property::dictionary {
				std::make_pair(keystring("a"), property("b")),
			});
			Assert::That(prop.begin(), Is().Not().EqualTo(prop2.begin()));
			Assert::That(prop2.begin(), Is().Not().EqualTo(prop.begin()));
		}
		Spec(compare_value_in_containers) {
			property prop(property::dictionary {
				std::make_pair(keystring("a"), property("b")),
			});
			const property prop2(property::array {
				std::make_pair(0, property("a")),
				std::make_pair(1, property("b")),
			});
			Assert::That(*prop.begin(), Is().Not().EqualTo(*prop2.begin()));
			Assert::That(*prop.begin(), Is().EqualTo(*(++prop2.begin())));
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

Describe(test_merge) {
	Context(both_props_are_none) {
		property original;
		property admixture;
		property result = property::merge(original, admixture);
		Spec(result_is_none) {
			Assert::That(result.is_none(), Is().EqualTo(true));
		}
	};
	Context(original_is_none) {
		property original;
		property admixture = property(7);
		property result = property::merge(original, admixture);
		Spec(result_is_admixture) {
			Assert::That(result.is_int(), Is().EqualTo(true));
			Assert::That(result.as_int(), Is().EqualTo(7));
		}
	};
	Context(admixture_is_none) {
		property original = property("xexe");
		property admixture;
		property result = property::merge(original, admixture);
		Spec(result_is_original) {
			Assert::That(result.is_string(), Is().EqualTo(true));
			Assert::That(result.as_string(), Is().EqualTo("xexe"));
		}
	};
	Context(cast_exceptions) {
		When(float_double) {
			property original = property(7.);
			property admixture = property(7.f);
			Then(result_is_none) {
				AssertThrowsEx(PropertyCastException, property::merge(original, admixture));
			}
		};
		When(long_string) {
			property original = property(7L);
			property admixture = property(std::string("zskhg"));
			Then(result_is_none) {
				AssertThrowsEx(PropertyCastException, property::merge(original, admixture));
			}
		};
		When(dict_array) {
			property original = property(property::array {});
			property admixture = property(property::dictionary {});
			Then(result_is_none) {
				AssertThrowsEx(PropertyCastException, property::merge(original, admixture));
			}
		};
		When(float_and_int_in_array) {
			property original = property(property::array {std::make_pair(0, property(7.f))});
			property admixture = property(property::array {std::make_pair(0, property(7))});
			Then(result_is_none) {
				AssertThrowsEx(PropertyCastException, property::merge(original, admixture));
			}
		};
		When(string_and_array_in_dict) {
			property original = property(property::dictionary {std::make_pair(keystring("gf"), property("sdhf"))});
			property admixture = property(property::dictionary {std::make_pair(keystring("gf"), property(property::array{}))});
			Then(result_is_none) {
				AssertThrowsEx(PropertyCastException, property::merge(original, admixture));
			}
		};
		When(merge_base) {
			Then(admixture_returns) {
				Assert::That(property::merge(property(1), property(2)).as_int(), Is().EqualTo(2));
				Assert::That(property::merge(property(3L), property(4L)).as_long(), Is().EqualTo(4L));
				Assert::That(property::merge(property(5.f), property(6.f)).as_float(), Is().EqualTo(6.f));
				Assert::That(property::merge(property(7.), property(5.)).as_double(), Is().EqualTo(5.));
				Assert::That(property::merge(property("key"), property(keystring("xxx"))).as_string(), Is().EqualTo("xxx"));
			}
		};
		When(merge_dictionary) {
			property original = property(property::dictionary {
				std::make_pair(keystring("gf"), property("sdhf")),
				std::make_pair(keystring("pos"), property(1.f)),
				std::make_pair(keystring("gasdf"), property(property::array {
					std::make_pair(std::size_t(0), property(3.f)),
				}))
			});
			property admixture = property(property::dictionary {
				std::make_pair(keystring("pos"), property(2.f)),
				std::make_pair(keystring("gasdf"), property(property::array {
					std::make_pair(std::size_t(0), property(4.f)),
				}))
			});
			property result = property::merge(original, admixture);
			Then(result_is_none) {
				Assert::That(result.is_dictionary(), Is().EqualTo(true));
				Assert::That(result["gf"].is_string(), Is().EqualTo(true));
				Assert::That(result["gasdf"].is_array(), Is().EqualTo(true));
				Assert::That(result["pos"].is_float(), Is().EqualTo(true));
				Assert::That(result["pos"].as_float(), Is().EqualTo(2.f));
				Assert::That(result["gasdf"][std::size_t(0)].is_float(), Is().EqualTo(true));
				Assert::That(result["gasdf"][std::size_t(0)].as_float(), Is().EqualTo(4.f));
			}
		};
		When(merge_array) {
			When(first_is_none) {
				property _1;
				property _2 = property(property::array {
					std::make_pair(std::size_t(0), property(14)),
					std::make_pair(std::size_t(1), property(15))
				});
				property result = property::merge(_1, _2);
				Then(_14_15) {
					Assert::That(result[std::size_t(0)].as_int(), Is().EqualTo(14));
					Assert::That(result[std::size_t(1)].as_int(), Is().EqualTo(15));
				}
			};
			When(first_less_second) {
				property _1 = property(property::array {
					std::make_pair(std::size_t(0), property(13))
				});
				property _2 = property(property::array {
					std::make_pair(std::size_t(0), property(14)),
					std::make_pair(std::size_t(1), property(15))
				});
				property result = property::merge(_1, _2);
				Then(_14_15) {
					Assert::That(result[std::size_t(0)].as_int(), Is().EqualTo(14));
					Assert::That(result[std::size_t(1)].as_int(), Is().EqualTo(15));
				}
			};
			When(first_greater_second) {
				property _1 = property(property::array {
					std::make_pair(std::size_t(0), property(14)),
					std::make_pair(std::size_t(1), property(15))
				});
				property _2 = property(property::array {
					std::make_pair(std::size_t(0), property(13))
				});
				property result = property::merge(_1, _2);
				Then(_13_15) {
					Assert::That(result[std::size_t(0)].as_int(), Is().EqualTo(13));
					Assert::That(result[std::size_t(1)].as_int(), Is().EqualTo(15));
				}
			};
			When(size_equals) {
				property _1 = property(property::array {
					std::make_pair(std::size_t(0), property(14)),
				});
				property _2 = property(property::array {
					std::make_pair(std::size_t(0), property(13))
				});
				property result = property::merge(_1, _2);
				Then(_13) {
					Assert::That(result[std::size_t(0)].as_int(), Is().EqualTo(13));
				}
			};
		};
	};
};

Describe(property_checks) {
	Context(string_copy_constructor) {
		property _1 = property("key");
		property _2 = property(_1);
		When(change_first) {
			void SetUp() {
				Parent()._1 = property("value");
			}
			It(second_not_changed) {
				Assert::That(Parent()._1.as_string(), Is().EqualTo("value"));
				Assert::That(Parent()._2.as_string(), Is().EqualTo("key"));
			}
		};
		When(change_second) {
			void SetUp() {
				Parent()._2 = property("value");
			}
			It(first_not_changed) {
				Assert::That(Parent()._1.as_string(), Is().EqualTo("key"));
				Assert::That(Parent()._2.as_string(), Is().EqualTo("value"));
			}
		};
	};
	Context(array_copy_constructor) {
		property _1 = property::array {
			std::make_pair(std::size_t(0), property(13))
		};
		property _2 = property(_1);
		When(change_first) {
			void SetUp() {
				Parent()._1[std::size_t(0)] = property("value");
			}
			It(second_not_changed) {
				Assert::That(Parent()._1[std::size_t(0)].as_string(), Is().EqualTo("value"));
				Assert::That(Parent()._2[std::size_t(0)].as_int(), Is().EqualTo(13));
			}
		};
		When(change_second) {
			void SetUp() {
				Parent()._2[std::size_t(0)] = property("value");
			}
			It(second_not_changed) {
				Assert::That(Parent()._1[std::size_t(0)].as_int(), Is().EqualTo(13));
				Assert::That(Parent()._2[std::size_t(0)].as_string(), Is().EqualTo("value"));
			}
		};
	};
	Context(dict_copy_constructor) {
		property _1 = property::dictionary {
			std::make_pair(keystring("key"), property(13))
		};
		property _2 = property(_1);
		When(change_first) {
			void SetUp() {
				Parent()._1["key"] = property("value");
			}
			It(second_not_changed) {
				Assert::That(Parent()._1["key"].as_string(), Is().EqualTo("value"));
				Assert::That(Parent()._2["key"].as_int(), Is().EqualTo(13));
			}
		};
		When(change_second) {
			void SetUp() {
				Parent()._2["key"] = property("value");
			}
			It(second_not_changed) {
				Assert::That(Parent()._1["key"].as_int() , Is().EqualTo(13));
				Assert::That(Parent()._2["key"].as_string(), Is().EqualTo("value"));
			}
		};
	};
	Context(constructors) {
		When(default_constructor) {
			property _1 = property("key");
			property _2;
			Then(first_and_second_are_not_equal) {
				Assert::That(_1, Is().Not().EqualTo(_2));
			}
			Then(second_is_none) {
				Assert::That(_2.is_none(), Is().EqualTo(true));
			}
		};
		When(call_copy_constructor) {
			property _1 = property("key");
			property _2 = property(_1);
			Then(first_and_second_are_equal) {
				Assert::That(_1, Is().EqualTo(_2));
			}
			Then(first_and_second_are_same) {
				Assert::That(_1.is_string(), Is().EqualTo(true));
				Assert::That(_1.as_string(), Is().EqualTo("key"));
				Assert::That(_2.is_string(), Is().EqualTo(true));
				Assert::That(_2.as_string(), Is().EqualTo("key"));
			}
		};
		When(call_copy_constructor_for_array) {
			const property::array arr{
				{0, "value_1"}, {1, "value_2"}, {2, "value_3"}
			};
			property _1 = property(arr);
			property _2 = property(_1);
			Then(first_and_second_are_equal) {
				Assert::That(_1, Is().EqualTo(_2));
				Assert::That(_2.as_array(), Is().EqualTo(arr));
			}
			When(first_changed) {
				void SetUp() {
					Parent()._1[1] = 15;
					Parent()._1[2] = "xxx";
				}
				Then(second_is_not) {
					Assert::That(Parent()._1, Is().Not().EqualTo(Parent()._2));
					Assert::That(Parent()._2.as_array(), Is().EqualTo(Parent().arr));
				}
			};
			When(first_type_changed) {
				void SetUp() {
					Parent()._1 = 15;
				}
				Then(second_still_dict) {
					Assert::That(Parent()._2.is_array(), Is().EqualTo(true));
					Assert::That(Parent()._2.as_array(), Is().EqualTo(Parent().arr));
				}
			};
		};
		When(call_copy_constructor_for_dict) {
			const property::dictionary dict{
				{"key_1", "value_1"}, {"key_2", "value_2"}, {"key_3", "value_3"}
			};
			property _1 = property(dict);
			property _2 = property(_1);
			Then(first_and_second_are_equal) {
				Assert::That(_1, Is().EqualTo(_2));
				Assert::That(_2.as_dictionary(), Is().EqualTo(dict));
			}
			When(first_changed) {
				void SetUp() {
					Parent()._1["key_1"] = 15;
					Parent()._1["key_4"] = "xxx";
				}
				Then(second_is_not) {
					Assert::That(Parent()._1, Is().Not().EqualTo(Parent()._2));
					Assert::That(Parent()._2.as_dictionary(), Is().EqualTo(Parent().dict));
				}
			};
			When(first_type_changed) {
				void SetUp() {
					Parent()._1 = 15;
				}
				Then(second_still_dict) {
					Assert::That(Parent()._2.is_dictionary(), Is().EqualTo(true));
					Assert::That(Parent()._2.as_dictionary(), Is().EqualTo(Parent().dict));
				}
			};
		};
		When(call_move_constructor_for_int) {
			property _1 = 42;
			property _2 = property(std::move(_1));
			Then(first_empty_and_second_value_is_key) {
				Assert::That(_1.is_none(), Is().EqualTo(true));
				Assert::That(_2.is_int(), Is().EqualTo(true));
				Assert::That(_2.as_int(), Is().EqualTo(42));
			}
		};
		When(call_move_constructor) {
			property _1 = property("key");
			property _2 = property(std::move(_1));
			Then(first_empty_and_second_value_is_key) {
				Assert::That(_1.is_none(), Is().EqualTo(true));
				Assert::That(_2.is_string(), Is().EqualTo(true));
				Assert::That(_2.as_string(), Is().EqualTo("key"));
			}
		};
		When(call_move_constructor_for_array) {
			const property::array arr{
				{0, "value_1"}, {1, "value_2"}, {2, "value_3"}
			};
			property _1 = property(arr);
			property _2 = property(std::move(_1));
			Then(first_empty_and_second_value_is_key) {
				Assert::That(_1.is_none(), Is().EqualTo(true));
				Assert::That(_2.is_array(), Is().EqualTo(true));
				Assert::That(_2.as_array(), Is().EqualTo(arr));
			}
		};
		When(call_move_constructor_for_dict) {
			const property::dictionary dict{
				{"key_1", "value_1"}, {"key_2", "value_2"}, {"key_3", "value_3"}
			};
			property _1 = property(dict);
			property _2 = property(std::move(_1));
			Then(first_empty_and_second_value_is_key) {
				Assert::That(_1.is_none(), Is().EqualTo(true));
				Assert::That(_2.is_dictionary(), Is().EqualTo(true));
				Assert::That(_2.as_dictionary(), Is().EqualTo(dict));
			}
		};
	};
	Context(assigns) {
		When(call_copy_assign) {
			property _1 = property("key");
			property _2;
			void SetUp() {
				_2 = _1;
			}
			Then(first_and_second_are_equal) {
				Assert::That(_1, Is().EqualTo(_2));
			}
			Then(first_and_second_are_same) {
				Assert::That(_1.is_string(), Is().EqualTo(true));
				Assert::That(_1.as_string(), Is().EqualTo("key"));
				Assert::That(_2.is_string(), Is().EqualTo(true));
				Assert::That(_2.as_string(), Is().EqualTo("key"));
			}
		};
		When(call_copy_assign_for_array) {
			const property::array arr{
				{0, "value_1"}, {1, "value_2"}, {2, "value_3"}
			};
			property _1 = property(arr);
			property _2;
			void SetUp() {
				_2 = _1;
			}
			Then(first_and_second_are_equal) {
				Assert::That(_1, Is().EqualTo(_2));
				Assert::That(_2.as_array(), Is().EqualTo(arr));
			}
			When(first_changed) {
				void SetUp() {
					Parent()._1[1] = 15;
					Parent()._1[2] = "xxx";
				}
				Then(second_is_not) {
					Assert::That(Parent()._1, Is().Not().EqualTo(Parent()._2));
					Assert::That(Parent()._2.as_array(), Is().EqualTo(Parent().arr));
				}
			};
			When(first_type_changed) {
				void SetUp() {
					Parent()._1 = 15;
				}
				Then(second_still_dict) {
					Assert::That(Parent()._2.is_array(), Is().EqualTo(true));
					Assert::That(Parent()._2.as_array(), Is().EqualTo(Parent().arr));
				}
			};
		};
		When(call_copy_assign_for_dict) {
			const property::dictionary dict{
				{"key_1", "value_1"}, {"key_2", "value_2"}, {"key_3", "value_3"}
			};
			property _1 = property(dict);
			property _2;
			void SetUp() {
				_2 = _1;
			}
			Then(first_and_second_are_equal) {
				Assert::That(_1, Is().EqualTo(_2));
				Assert::That(_2.as_dictionary(), Is().EqualTo(dict));
			}
			When(first_changed) {
				void SetUp() {
					Parent()._1["key_1"] = 15;
					Parent()._1["key_4"] = "xxx";
				}
				Then(second_is_not) {
					Assert::That(Parent()._1, Is().Not().EqualTo(Parent()._2));
					Assert::That(Parent()._2.as_dictionary(), Is().EqualTo(Parent().dict));
				}
			};
			When(first_type_changed) {
				void SetUp() {
					Parent()._1 = 15;
				}
				Then(second_still_dict) {
					Assert::That(Parent()._2.is_dictionary(), Is().EqualTo(true));
					Assert::That(Parent()._2.as_dictionary(), Is().EqualTo(Parent().dict));
				}
			};
		};
		When(call_move_assign_for_int) {
			property _1 = 17;
			property _2;
			void SetUp() {
				_2 = std::move(_1);
			}
			Then(first_empty_and_second_value_is_key) {
				Assert::That(_1.is_none(), Is().EqualTo(true));
				Assert::That(_2.is_int(), Is().EqualTo(true));
				Assert::That(_2.as_int(), Is().EqualTo(17));
			}
		};
		When(call_move_assign) {
			property _1 = property("key");
			property _2;
			void SetUp() {
				_2 = std::move(_1);
			}
			Then(first_empty_and_second_value_is_key) {
				Assert::That(_1.is_none(), Is().EqualTo(true));
				Assert::That(_2.is_string(), Is().EqualTo(true));
				Assert::That(_2.as_string(), Is().EqualTo("key"));
			}
		};
		When(call_move_assign_for_array) {
			const property::array arr{
				{0, "value_1"}, {1, "value_2"}, {2, "value_3"}
			};
			property _1 = property(arr);
			property _2;
			void SetUp() {
				_2 = std::move(_1);
			}
			Then(first_empty_and_second_value_is_key) {
				Assert::That(_1.is_none(), Is().EqualTo(true));
				Assert::That(_2.is_array(), Is().EqualTo(true));
				Assert::That(_2.as_array(), Is().EqualTo(arr));
			}
		};
		When(call_move_assign_for_dict) {
			const property::dictionary dict{
				{"key_1", "value_1"}, {"key_2", "value_2"}, {"key_3", "value_3"}
			};
			property _1 = property(dict);
			property _2;
			void SetUp() {
				_2 = std::move(_1);
			}
			Then(first_empty_and_second_value_is_key) {
				Assert::That(_1.is_none(), Is().EqualTo(true));
				Assert::That(_2.is_dictionary(), Is().EqualTo(true));
				Assert::That(_2.as_dictionary(), Is().EqualTo(dict));
			}
		};
	};
	Context(size) {
		property _0;
		property _1 = 15;
		property _2 = "1234";
		property _3 = property::array {
			{0, "value_1"}, {1, "value_2"}
		};
		property _4 = property::dictionary {
			{"key_1", "value_1"}, {"key_2", "value_2"}, {"key_3", "value_3"}
		};
		Spec(none) {
			Assert::That(_0.size(), Is().EqualTo(0U));
		}
		Spec(integer) {
			AssertThrowsEx(PropertyCastException, _1.size());
		}
		Spec(string) {
			Assert::That(_2.size(), Is().EqualTo(4U));
		}
		Spec(array) {
			Assert::That(_3.size(), Is().EqualTo(2U));
		}
		Spec(dictionary) {
			Assert::That(_4.size(), Is().EqualTo(3U));
		}
	};
	Context(remove) {
		When(remove_from_none) {
			property _1;
			Then(exception_occured) {
				AssertThrowsEx(PropertyCastException, _1.remove(0));
				AssertThrowsEx(PropertyCastException, _1.remove("11"));
			}
		};
		When(remove_from_int) {
			property _1 = 15;
			Then(exception_occured) {
				AssertThrowsEx(PropertyCastException, _1.remove(0));
				AssertThrowsEx(PropertyCastException, _1.remove("11"));
			}
		};
		Context(remove_from_string) {
			property _1 = "th";
			Then(exception_occured) {
				AssertThrowsEx(PropertyCastException, _1.remove(0));
				AssertThrowsEx(PropertyCastException, _1.remove("11"));
			}
		};
		Context(remove_from_array) {
			property _1 = property::array {
				{0, "value_1"}, {1, "value_2"}, {2, "value_3"}
			};
			Then(remove_by_string_is_unavailable) {
				AssertThrowsEx(PropertyCastException, _1.remove("11"));
			}
		};
		Context(remove_from_dict) {
			property _2 = property::dictionary {
				{"key_1", "value_1"}, {"key_2", "value_2"}, {"key_3", "value_3"}
			};
			Then(remove_by_pos_is_unavailable) {
				property _1 = property::dictionary {};
				AssertThrowsEx(PropertyCastException, _1.remove(0));
			}
			When(remove_from_empty) {
				property _1 = property::dictionary {};
				Then(OutOfRangeException_thrown) {
					AssertThrowsEx(PropertyOutOfRangeException, _1.remove("123"));
				}
			};
			When(remove_non_exists) {
				Then(OutOfRangeException_thrown) {
					AssertThrowsEx(PropertyOutOfRangeException, Parent()._2.remove("123"));
				}
			};
			When(remove_exists) {
				void SetUp() {
					Parent()._2.remove("key_1");
				}
				Then(size_is_2) {
					Assert::That(Parent()._2.size(), Is().EqualTo(2U));
				}
				Then(item_is_removed) {
					const auto test = property::dictionary {
						{"key_2", "value_2"}, {"key_3", "value_3"}
					};
					Assert::That(Parent()._2.as_dictionary(), Is().EqualTo(test));
				}
			};
		};
	};
};

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
			AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, _0[0x1000030]);
			AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, _0[-1]);
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
			AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, _0[0x1000030]);
			AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, _0[-1]);
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

//TODO: More tests!

