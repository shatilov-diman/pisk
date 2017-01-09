// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module os of the project pisk.
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

#include <pisk/os/SysEventDispatcher.h>

using namespace igloo;

struct TestEvent
{
	int msg;
};

class TestEventHandler :
	public pisk::os::SysEventHandler<TestEvent>
{
	TestEvent last_event;

	virtual void handle(const TestEvent& event) final override
	{
		last_event = event;
	}
public:
	explicit TestEventHandler(pisk::os::SysEventDispatcherPtr<TestEvent> dispatcher):
		pisk::os::SysEventHandler<TestEvent>(dispatcher),
		last_event {0}
	{}

	TestEvent get_last_event()
	{
		return std::move(last_event);
	}
};

static pisk::os::SysEventDispatcherPtr<TestEvent> CreateEventDispatcher()
{
	auto dispatcher = std::make_shared<pisk::os::SysEventDispatcher<TestEvent>>();

	return {nullptr, dispatcher};
}

Describe(TestSysEventDispatcher) {
	pisk::os::SysEventDispatcherPtr<TestEvent> dispatcher = CreateEventDispatcher();

	When(dispatch_event_without_listeners) {
		Then(no_crash) {
			Root().dispatcher->dispatch({13});
		}
	};
	When(one_handler_registered) {
		std::unique_ptr<TestEventHandler> handler_1;
		void SetUp() {
			handler_1 = std::make_unique<TestEventHandler>(Root().dispatcher);
		}
		When(dispatch_event) {
			TestEvent event = TestEvent {13};
			void SetUp() {
				Root().dispatcher->dispatch(event);
			}
			Then(handler_1_returns_event) {
				Assert::That(Parent().handler_1->get_last_event().msg, Is().EqualTo(13));
			}
		};
		When(second_handler_registered) {
			std::unique_ptr<TestEventHandler> handler_2;
			void SetUp() {
				handler_2 = std::make_unique<TestEventHandler>(Root().dispatcher);
			}
			When(dispatch_event) {
				TestEvent event = TestEvent {42};
				void SetUp() {
					Root().dispatcher->dispatch(event);
				}
				Then(handler_1_returns_event) {
					Assert::That(Parent().Parent().handler_1->get_last_event().msg, Is().EqualTo(event.msg));
				}
				Then(handler_2_returns_event) {
					Assert::That(Parent().handler_2->get_last_event().msg, Is().EqualTo(42));
				}
			};
			When(first_unregistered) {
				void SetUp() {
					Parent().Parent().handler_1.reset();
				}
				When(dispatch_event) {
					TestEvent event = TestEvent {88};
					void SetUp() {
						Root().dispatcher->dispatch(event);
					}
					Then(handler_2_returns_event) {
						Assert::That(Parent().Parent().handler_2->get_last_event().msg, Is().EqualTo(88));
					}
				};
			};
		};
	};
};

