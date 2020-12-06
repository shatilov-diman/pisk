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
			When(remove_from_empty) {
				property _2 = property::array {};
				Then(OutOfRangeException_thrown) {
					AssertThrowsEx(PropertyOutOfRangeException, _2.remove(1U));
				}
			};
			When(remove_nonexisten) {
				Then(OutOfRangeException_thrown) {
					AssertThrowsEx(PropertyOutOfRangeException, Parent()._1.remove(4U));
				}
			};
			When(remove_exists) {
				When(remove_first) {
					void SetUp() {
						Parent().Parent()._1.remove(0U);
					}
					Then(size_is_2) {
						Assert::That(Parent().Parent()._1.size(), Is().EqualTo(2U));
					}
					Then(item_is_removed) {
						const auto test = property::array {
							{0U, "value_2"}, {1U, "value_3"}
						};
						Assert::That(Parent().Parent()._1.as_array(), Is().EqualTo(test));
					}
				};
				When(remove_middle) {
					void SetUp() {
						Parent().Parent()._1.remove(1U);
					}
					Then(size_is_2) {
						Assert::That(Parent().Parent()._1.size(), Is().EqualTo(2U));
					}
					Then(item_is_removed) {
						const auto test = property::array {
							{0U, "value_1"}, {1U, "value_3"}
						};
						Assert::That(Parent().Parent()._1.as_array(), Is().EqualTo(test));
					}
				};
				When(remove_last) {
					void SetUp() {
						Parent().Parent()._1.remove(2U);
					}
					Then(size_is_2) {
						Assert::That(Parent().Parent()._1.size(), Is().EqualTo(2U));
					}
					Then(item_is_removed) {
						const auto test = property::array {
							{0U, "value_1"}, {1U, "value_2"}
						};
						Assert::That(Parent().Parent()._1.as_array(), Is().EqualTo(test));
					}
				};
			};
		};
		Context(remove_from_broken_array) {
			property _1 = property::array {
				{0U, "value_1"}, {5U, "value_3"}
			};
			Then(OutOfRangeException_thrown) {
				AssertThrowsEx(PropertyOutOfRangeException, _1.remove(4U));
				AssertThrowsEx(PropertyOutOfRangeException, _1.remove(5U));
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

