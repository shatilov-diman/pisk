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
#include <pisk/utils/enum_iterator.h>

#include <functional>

using namespace igloo;
using namespace pisk::utils;

Describe(enum_iterator_range) {
	Context(one_element) {
		enum class Test {
			first_element,
		};
		Spec(item_enumerated) {
			int count = 0;
			for (const Test el : iterators::range(Test::first_element, Test::first_element))
			{
				UNUSED(el);
				++count;
			}
			Assert::That(count, Is().EqualTo(1));
		}
	};
	Context(two_elements) {
		enum class Test {
			first_element,
			last_element,
		};
		Spec(item_enumerated) {
			int count = 0;
			for (const Test el : iterators::range(Test::first_element, Test::last_element))
			{
				UNUSED(el);
				++count;
			}
			Assert::That(count, Is().EqualTo(2));
		}
	};
	Context(two_elements_reverse) {
		enum class Test {
			first_element,
			last_element,
		};
		Spec(item_enumerated) {
			int count = 0;
			for (const Test el : iterators::range(Test::last_element, Test::first_element))
			{
				UNUSED(el);
				++count;
			}
			Assert::That(count, Is().EqualTo(2));
		}
	};
	Context(two_elements_with_holes) {
		enum class Test {
			first_element = 0,
			last_element = 10,
		};
		Spec(item_enumerated) {
			int count = 0;
			for (const Test el : iterators::range(Test::first_element, Test::last_element))
			{
				UNUSED(el);
				++count;
			}
			Assert::That(count, Is().EqualTo(11));
		}
	};
	Context(two_elements_with_holes_reverse) {
		enum class Test {
			first_element = 0,
			last_element = 10,
		};
		Spec(item_enumerated) {
			int count = 0;
			for (const Test el : iterators::range(Test::last_element, Test::first_element))
			{
				UNUSED(el);
				++count;
			}
			Assert::That(count, Is().EqualTo(11));
		}
	};
	Context(three_elements) {
		enum class Test {
			first_element,
			second_element,
			last_element,
		};
		Spec(item_enumerated) {
			int count = 0;
			for (const Test el : iterators::range(Test::first_element, Test::last_element))
			{
				UNUSED(el);
				++count;
			}
			Assert::That(count, Is().EqualTo(3));
		}
	};
	Context(three_elements_reverse) {
		enum class Test {
			first_element,
			second_element,
			last_element,
		};
		Spec(item_enumerated) {
			int count = 0;
			for (const Test el : iterators::range(Test::last_element, Test::first_element))
			{
				UNUSED(el);
				++count;
			}
			Assert::That(count, Is().EqualTo(3));
		}
	};
};

Describe(enum_iterator) {
	enum class Test {
		first_element,
		second_element,
		last_element,
	};
	iterators::enum_iterator<Test> it;
	iterators::enum_iterator<Test> it2;
	iterators::enum_iterator<Test> end;
	Context(test_end) {
		When(dereference) {
			Then(exception_thrown) {
				AssertThrowsEx(pisk::infrastructure::OutOfRangeException, *Root().end);
			}
		};
		When(increment) {
			Then(exception_thrown) {
				AssertThrowsEx(pisk::infrastructure::OutOfRangeException, ++Root().end);
			}
		};
	};
	Context(iterator_from_last_and_last) {
		void SetUp() {
			Root().it = iterators::enum_iterator<Test>(Test::last_element, Test::last_element);
		}

		Spec(it_is_not_equal_to_end) {
			Assert::That(Root().it, Is().Not().EqualTo(Root().end));
		}
		When(dereference) {
			Then(value_is_eq_to_last_element) {
				Assert::That(*Root().it, Is().EqualTo(Test::last_element));
			}
		};
		When(increment) {
			void SetUp() {
				++Root().it;
			}
			Then(it_is_equal_to_end) {
				Assert::That(Root().it, Is().EqualTo(Root().end));
			}
			When(dereference) {
				Then(exception_thrown) {
					AssertThrowsEx(pisk::infrastructure::OutOfRangeException, *Root().it);
				}
			};
			When(increment_again) {
				Then(exception_thrown) {
					AssertThrowsEx(pisk::infrastructure::OutOfRangeException, ++Root().it);
				}
			};
		};
	};
	Context(iterator_from_first_and_last) {
		void SetUp() {
			Root().it = iterators::enum_iterator<Test>(Test::first_element, Test::last_element);
			Root().it2 = iterators::enum_iterator<Test>(Test::first_element, Test::last_element);
		}

		Spec(it_is_not_equal_to_end) {
			Assert::That(Root().it, Is().Not().EqualTo(Root().end));
		}
		Spec(it_is_equal_to_it2) {
			Assert::That(Root().it, Is().EqualTo(Root().it2));
		}
		When(dereference) {
			Then(value_is_eq_to_first_element) {
				Assert::That(*Root().it, Is().EqualTo(Test::first_element));
			}
		};
		When(increment) {
			void SetUp() {
				++Root().it;
			}
			Then(it_is_not_equal_to_end) {
				Assert::That(Root().it, Is().Not().EqualTo(Root().end));
			}
			Spec(it_is_not_equal_to_it2) {
				Assert::That(Root().it, Is().Not().EqualTo(Root().it2));
			}
			Spec(it_is_equal_to_it_from_second_second) {
				iterators::enum_iterator<Test> it_second(Test::second_element, Test::second_element);
				Assert::That(Root().it, Is().EqualTo(it_second));
			}
			Spec(it_is_equal_to_it_from_second_last) {
				iterators::enum_iterator<Test> it_second(Test::second_element, Test::last_element);
				Assert::That(Root().it, Is().EqualTo(it_second));
			}
			When(dereference) {
				Then(value_is_eq_to_second_element) {
					Assert::That(*Root().it, Is().EqualTo(Test::second_element));
				}
			};
			When(double_increment) {
				Then(it_eq_to_end) {
					Assert::That(++++Root().it, Is().EqualTo(Root().end));
				}
			};
		};
	};
};

