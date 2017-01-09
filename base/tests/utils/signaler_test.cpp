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
#include <pisk/utils/signaler.h>

#include <functional>
#include <exception>
#include <set>

using namespace igloo;
using namespace pisk::utils;

class TestKeyboardDevice
{
public:
	struct IOKeyEvent
	{
		std::size_t vkey;
	};

	signaler<IOKeyEvent> KeyDown;
	signaler<IOKeyEvent> KeyUp;

	signaler<void> Event1;
	signaler<void> Event2;
};

Describe(signaler_test) {
	TestKeyboardDevice keyboard;

	When(emit_without_subscribtions) {
		Then(no_any_heppend) {
			TestKeyboardDevice::IOKeyEvent evnt = {__LINE__};
			Root().keyboard.KeyDown.emit(evnt);
		}
	};
	When(subscribe_to_the_signaler) {
		const std::size_t num = __LINE__;
		std::size_t vkey = 0;
		void SetUp() {
			Root().keyboard.KeyDown += [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
				vkey = evnt.vkey;
			};
			TestKeyboardDevice::IOKeyEvent evnt = {num};
			Root().keyboard.KeyDown.emit(evnt);
		}
		Then(begin_receive_signals) {
			Assert::That(vkey, Is().EqualTo(num));
		}
	};
	Context(subscribtion_to_the_keyboard) {
		std::size_t vkey = 0;
		void SetUp() {
			Root().keyboard.KeyDown += [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
				vkey = evnt.vkey;
			};
		}
		When(emit_event) {
			const std::size_t num = __LINE__;
			void SetUp() {
				TestKeyboardDevice::IOKeyEvent evnt = {num};
				Root().keyboard.KeyDown.emit(evnt);
			}
			Then(vkey_is_expected) {
				Assert::That(Parent().vkey, Is().EqualTo(num));
			}
		};
		When(unsubscribe) {
			const std::size_t num = __LINE__;
			std::size_t vkey = 0;
			signaler<TestKeyboardDevice::IOKeyEvent>::eventhandler handler;
			void SetUp() {
				handler = [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
					vkey = evnt.vkey;
				};
				Root().keyboard.KeyDown += handler;
				TestKeyboardDevice::IOKeyEvent evnt = {num};
				Root().keyboard.KeyDown.emit(evnt);
				Root().keyboard.KeyDown -= handler;
			}
			Then(emit_does_not_notify_us) {
				Root().keyboard.KeyDown.emit({__LINE__});
				Assert::That(vkey, Is().EqualTo(num));
			}
		};
	};
	Context(two_subscribtion) {
		std::size_t vkey1 = 0;
		std::size_t vkey2 = 0;
		signaler<TestKeyboardDevice::IOKeyEvent>::eventhandler handler1;
		signaler<TestKeyboardDevice::IOKeyEvent>::eventhandler handler2;
		void SetUp() {
			handler1 = [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
				vkey1 = evnt.vkey;
			};
			handler2 = [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
				vkey2 = evnt.vkey;
			};
		}
		When(subscribe_first) {
			const std::size_t num = __LINE__;
			void SetUp() {
				Root().keyboard.KeyDown += Parent().handler1;
			}
			Then(emittion_notifies_first_and_doesnot_notify_second) {
				Root().keyboard.KeyDown.emit({num});
				Assert::That(Parent().vkey1, Is().EqualTo(num));
				Assert::That(Parent().vkey2, Is().EqualTo(0U));
			}
		};
		When(subscribe_both) {
			const std::size_t num = __LINE__;
			void SetUp() {
				Root().keyboard.KeyDown += Parent().handler1;
				Root().keyboard.KeyDown += Parent().handler2;
			}
			Then(emittion_notifies_both) {
				Root().keyboard.KeyDown.emit({num});
				Assert::That(Parent().vkey1, Is().EqualTo(num));
				Assert::That(Parent().vkey2, Is().EqualTo(num));
			}
			When(unsubscribe_first) {
				const std::size_t num = __LINE__;
				void SetUp() {
					Root().keyboard.KeyDown -= Parent().Parent().handler1;
				}
				Then(second_still_notified) {
					Root().keyboard.KeyDown.emit({num});
					Assert::That(Parent().Parent().vkey1, Is().EqualTo(0U));
					Assert::That(Parent().Parent().vkey2, Is().EqualTo(num));
				}
			};
		};
	};
	Context(independent_subscribtions) {
		std::size_t vkey1 = 0;
		std::size_t vkey2 = 0;
		signaler<TestKeyboardDevice::IOKeyEvent>::eventhandler handler1;
		signaler<TestKeyboardDevice::IOKeyEvent>::eventhandler handler2;
		void SetUp() {
			handler1 = [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
				vkey1 = evnt.vkey;
			};
			handler2 = [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
				vkey2 = evnt.vkey;
			};
			Root().keyboard.KeyDown += handler1;
			Root().keyboard.KeyUp += handler2;
		}
		When(emit_one) {
			const std::size_t num = __LINE__;
			void SetUp() {
				Root().keyboard.KeyDown.emit({num});
			}
			Then(only_first_notified) {
					Assert::That(Parent().vkey1, Is().EqualTo(num));
					Assert::That(Parent().vkey2, Is().EqualTo(0U));
			}
		};
		When(emit_both) {
			const std::size_t num1 = __LINE__;
			const std::size_t num2 = __LINE__;
			void SetUp() {
				Root().keyboard.KeyDown.emit({num1});
				Root().keyboard.KeyUp.emit({num2});
			}
			Then(both_notified_with_different_values) {
					Assert::That(Parent().vkey1, Is().EqualTo(num1));
					Assert::That(Parent().vkey2, Is().EqualTo(num2));
			}
		};
	};
	Context(forward_subscribtions) {
		std::size_t vkey1 = 0;
		std::size_t vkey2 = 0;
		signaler<TestKeyboardDevice::IOKeyEvent>::eventhandler handler1;
		signaler<TestKeyboardDevice::IOKeyEvent>::eventhandler handler2;
		void SetUp() {
			handler1 = [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
				vkey1 = evnt.vkey;
			};
			handler2 = [this](const TestKeyboardDevice::IOKeyEvent& evnt) {
				vkey2 = evnt.vkey;
			};
		}
		When(subscribe_to_signaler) {
			const std::size_t num = __LINE__;
			void SetUp() {
				Root().keyboard.KeyDown += Parent().handler1;
				Root().keyboard.KeyUp += Root().keyboard.KeyDown;
				Root().keyboard.KeyDown += Parent().handler2;
			}
			Then(emittion_notifies_first_and_doesnot_notify_second) {
				Root().keyboard.KeyUp.emit({ num });
				Assert::That(Parent().vkey1, Is().EqualTo(num));
				Assert::That(Parent().vkey2, Is().EqualTo(num));
			}
		};
	};
	Context(void_signaler) {
		const std::size_t v1 = __LINE__;
		const std::size_t v2 = __LINE__;
		std::size_t vkey1 = 0;
		std::size_t vkey2 = 0;
		signaler<void>::eventhandler handler1;
		signaler<void>::eventhandler handler2;
		void SetUp() {
			handler1 = [this]() {
				vkey1 = v1;
			};
			handler2 = [this]() {
				vkey2 = v2;
			};
		}
		When(subscribe_to_signaler) {
			void SetUp() {
				Root().keyboard.Event1 += Parent().handler1;
				Root().keyboard.Event2 += Root().keyboard.Event1;
				Root().keyboard.Event1 += Parent().handler2;
			}
			Then(emittion_notifies_first_and_doesnot_notify_second) {
				Root().keyboard.Event1.emit();
				Assert::That(Parent().vkey1, Is().EqualTo(Parent().v1));
				Assert::That(Parent().vkey2, Is().EqualTo(Parent().v2));
			}
		};
	};
	Context(exceptions) {
		const std::size_t v1 = __LINE__;
		std::size_t vkey1 = 0;
		signaler<void>::eventhandler handler1;
		void SetUp() {
			handler1 = [this]() {
				throw std::runtime_error("");
			};
		}
		When(subscribe_to_signaler) {
			void SetUp() {
				Root().keyboard.Event1 += Parent().handler1;
			}
			Then(emittion_notifies_first_and_doesnot_notify_second) {
				AssertThrowsEx(std::runtime_error, Root().keyboard.Event1.emit());
			}
		};
	};
};

Describe(subscribtions) {
	TestKeyboardDevice keyboard;
	const std::size_t v1 = __LINE__;
	const std::size_t v2 = __LINE__;
	std::size_t vkey = 0;

	Context(subscribe_as_subscribtion) {
		auto_unsubscriber subscribtion = subscribe();
		auto_unsubscriber subscribe() {
			return Root().keyboard.KeyDown.subscribe([this](const TestKeyboardDevice::IOKeyEvent& event) {
				Root().vkey = event.vkey;
			});
		}
		void unsubscribe() {
			subscribtion.reset();
		}
		When(emit_event) {
			void SetUp() {
				TestKeyboardDevice::IOKeyEvent evnt = {Root().v1};
				Root().keyboard.KeyDown.emit(evnt);
			}
			Then(subscribtion_called) {
				Assert::That(Root().vkey, Is().EqualTo(Root().v1));
			}
		};
		When(_unsubscribe) {
			void SetUp() {
				Parent().unsubscribe();
			}
			Then(no_more_events_emitted) {
				TestKeyboardDevice::IOKeyEvent evnt = {Root().v2};
				Root().keyboard.KeyDown.emit(evnt);
				Assert::That(Root().vkey, Is().EqualTo(0U));
			}
		};
	};
	Context(auto_unsubscribe) {
		Spec(test_auto_unsubscribe) {
			{
				auto_unsubscriber subscribtion =
					Root().keyboard.KeyDown.subscribe([this](const TestKeyboardDevice::IOKeyEvent& event) {
						Root().vkey = event.vkey;
					});
				TestKeyboardDevice::IOKeyEvent evnt = {Root().v1};
				Root().keyboard.KeyDown.emit(evnt);
				Assert::That(Root().vkey, Is().EqualTo(Root().v1));
			}
			{
				TestKeyboardDevice::IOKeyEvent evnt = {Root().v2};
				Root().keyboard.KeyDown.emit(evnt);
				Assert::That(Root().vkey, Is().EqualTo(Root().v1));
			}
		}
	};
};

