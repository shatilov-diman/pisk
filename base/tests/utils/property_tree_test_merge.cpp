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

Describe(test_merge) {
	Context(both_props_are_none) {
		Spec(result_is_none) {
			property original;
			property::replace(original, property{});
			Assert::That(original.is_none(), Is().EqualTo(true));
		}
	};
	Context(original_is_none) {
		Spec(result_is_admixture) {
			property original;
			property::replace(original, property(7));
			Assert::That(original.is_int(), Is().EqualTo(true));
			Assert::That(original.as_int(), Is().EqualTo(7));
		}
	};
	Context(admixture_is_none) {
		Spec(result_is_original) {
			property original = property("xexe");
			property::replace(original, property{});
			Assert::That(original.is_string(), Is().EqualTo(true));
			Assert::That(original.as_string(), Is().EqualTo("xexe"));
		}
	};
	Context(cast_exceptions) {
		When(float_double) {
			Then(result_is_none) {
				property original = property(7.);
				AssertThrowsEx(PropertyCastException, property::replace(original, property(7.f)));
			}
		};
		When(long_string) {
			Then(result_is_none) {
				property original = property(7L);
				AssertThrowsEx(PropertyCastException, property::replace(original, property(std::string("zskhg"))));
			}
		};
		When(dict_array) {
			Then(result_is_none) {
				property original = property(property::array {});
				AssertThrowsEx(PropertyCastException, property::replace(original, property(property::dictionary {})));
			}
		};
		When(float_and_int_in_array) {
			property original = property(property::array {std::make_pair(0, property(7.f))});
			property admixture = property(property::array {std::make_pair(0, property(7))});
			Then(result_is_none) {
				AssertThrowsEx(PropertyCastException, property::replace(original, std::move(admixture)));
			}
		};
		When(string_and_array_in_dict) {
			property original = property(property::dictionary {std::make_pair(keystring("gf"), property("sdhf"))});
			property admixture = property(property::dictionary {std::make_pair(keystring("gf"), property(property::array{}))});
			Then(result_is_none) {
				AssertThrowsEx(PropertyCastException, property::replace(original, std::move(admixture)));
			}
		};
		When(merge_base) {
			Then(replace_rvalue) {
				property _1(1), _2(3L), _3(5.f), _4(7.), _5("key");
				property::replace(_1, property(2));
				property::replace(_2, property(4L));
				property::replace(_3, property(6.f));
				property::replace(_4, property(5.));
				property::replace(_5, property(keystring("xxx")));
				Assert::That(_1.as_int(), Is().EqualTo(2));
				Assert::That(_2.as_long(), Is().EqualTo(4L));
				Assert::That(_3.as_float(), Is().EqualTo(6.f));
				Assert::That(_4.as_double(), Is().EqualTo(5.));
				Assert::That(_5.as_string(), Is().EqualTo("xxx"));
			}
			Then(replace_lvalue) {
				property _1(1), _2(3L), _3(5.f), _4(7.), _5("key");
				property __1(2), __2(4L), __3(6.f), __4(5.), __5("xxx");
				property::replace(_1, __1);
				property::replace(_2, __2);
				property::replace(_3, __3);
				property::replace(_4, __4);
				property::replace(_5, __5);
				Assert::That(_1.as_int(), Is().EqualTo(2));
				Assert::That(_2.as_long(), Is().EqualTo(4L));
				Assert::That(_3.as_float(), Is().EqualTo(6.f));
				Assert::That(_4.as_double(), Is().EqualTo(5.));
				Assert::That(_5.as_string(), Is().EqualTo("xxx"));
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
			Then(replace_lvalue) {
				property::replace(original, admixture);
				Assert::That(original.is_dictionary(), Is().EqualTo(true));
				Assert::That(original["gf"].is_string(), Is().EqualTo(true));
				Assert::That(original["gasdf"].is_array(), Is().EqualTo(true));
				Assert::That(original["pos"].is_float(), Is().EqualTo(true));
				Assert::That(original["pos"].as_float(), Is().EqualTo(2.f));
				Assert::That(original["gasdf"][std::size_t(0)].is_float(), Is().EqualTo(true));
				Assert::That(original["gasdf"][std::size_t(0)].as_float(), Is().EqualTo(4.f));
			}
			Then(replace_rvalue) {
				property::replace(original, std::move(admixture));
				Assert::That(original.is_dictionary(), Is().EqualTo(true));
				Assert::That(original["gf"].is_string(), Is().EqualTo(true));
				Assert::That(original["gasdf"].is_array(), Is().EqualTo(true));
				Assert::That(original["pos"].is_float(), Is().EqualTo(true));
				Assert::That(original["pos"].as_float(), Is().EqualTo(2.f));
				Assert::That(original["gasdf"][std::size_t(0)].is_float(), Is().EqualTo(true));
				Assert::That(original["gasdf"][std::size_t(0)].as_float(), Is().EqualTo(4.f));
			}
		};
		When(merge_array) {
			When(first_is_none) {
				property _1;
				property _2 = property(property::array {
					std::make_pair(std::size_t(0), property(14)),
					std::make_pair(std::size_t(1), property(15))
				});
				Then(_14_15_mv) {
					property::replace(_1, std::move(_2));
					Assert::That(_1[std::size_t(0)].as_int(), Is().EqualTo(14));
					Assert::That(_1[std::size_t(1)].as_int(), Is().EqualTo(15));
					Assert::That(_2.is_none(), Is().EqualTo(true));
				}
				Then(_14_15_cpy) {
					property::replace(_1, _2);
					Assert::That(_1[std::size_t(0)].as_int(), Is().EqualTo(14));
					Assert::That(_1[std::size_t(1)].as_int(), Is().EqualTo(15));
					Assert::That(_2[std::size_t(0)].as_int(), Is().EqualTo(14));
					Assert::That(_2[std::size_t(1)].as_int(), Is().EqualTo(15));
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
				Then(_14_15) {
					property::replace(_1, std::move(_2));
					Assert::That(_1[std::size_t(0)].as_int(), Is().EqualTo(14));
					Assert::That(_1[std::size_t(1)].as_int(), Is().EqualTo(15));
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
				Then(_13_15) {
					property::replace(_1, std::move(_2));
					Assert::That(_1[std::size_t(0)].as_int(), Is().EqualTo(13));
					Assert::That(_1[std::size_t(1)].as_int(), Is().EqualTo(15));
				}
			};
			When(size_equals) {
				property _1 = property(property::array {
					std::make_pair(std::size_t(0), property(14)),
				});
				property _2 = property(property::array {
					std::make_pair(std::size_t(0), property(13))
				});
				Then(_13) {
					property::replace(_1, std::move(_2));
					Assert::That(_1[std::size_t(0)].as_int(), Is().EqualTo(13));
				}
			};
		};
	};
};

