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
	const bool mark_handled;

	virtual bool handle(const TestEvent& event) final override
	{
		last_event = event;
		return mark_handled;
	}
public:
	explicit TestEventHandler(pisk::os::SysEventDispatcherPtr<TestEvent> dispatcher, const bool mark_handled = false):
		pisk::os::SysEventHandler<TestEvent>(dispatcher),
		last_event {0},
		mark_handled(mark_handled)
	{}

	TestEvent get_last_event()
	{
		return std::move(last_event);
	}
};

std::shared_ptr<pisk::os::SysEventHandler<TestEvent>> external_last_link;

class TestEventHandlerWithUnsubscribe :
	public pisk::os::SysEventHandler<TestEvent>
{
	virtual bool handle(const TestEvent&) final override
	{
		external_last_link.reset();
		return false;
	}
public:
	explicit TestEventHandlerWithUnsubscribe(pisk::os::SysEventDispatcherPtr<TestEvent> dispatcher):
		pisk::os::SysEventHandler<TestEvent>(dispatcher)
	{}
};

static pisk::os::SysEventDispatcherPtr<TestEvent> CreateEventDispatcher()
{
	auto remote = std::make_shared<pisk::os::MainLoopRemoteTasks>();
	auto dispatcher = std::make_shared<pisk::os::SysEventDispatcher<TestEvent>>(pisk::os::MainLoopRemoteTasksPtr{nullptr, remote});

	return {nullptr, dispatcher};
}

Describe(TestSysEventDispatcher) {
	pisk::os::SysEventDispatcherPtr<TestEvent> dispatcher = CreateEventDispatcher();

	When(dispatch_event_without_listeners) {
		Then(no_crash) {
			const bool result = Root().dispatcher->dispatch({13});
			Assert::That(result, Is().EqualTo(false));
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
				const bool result = Root().dispatcher->dispatch(event);
				Assert::That(result, Is().EqualTo(false));
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
	When(someone_mark_event_as_handled) {
		std::vector<std::unique_ptr<TestEventHandler>> mark_handlers;
		std::vector<std::unique_ptr<TestEventHandler>> unmark_handlers;
		TestEvent event = TestEvent {13};
		void SetUp() {
			for (std::size_t index = 0; index < 32; ++index)
			{
				mark_handlers.emplace_back(std::make_unique<TestEventHandler>(Root().dispatcher, true));
				unmark_handlers.emplace_back(std::make_unique<TestEventHandler>(Root().dispatcher, false));
			}
			const bool result = Root().dispatcher->dispatch(event);
			Assert::That(result, Is().EqualTo(true));
		}
		Then(all_should_receiv_event) {
			for (const auto& handler : mark_handlers)
				Assert::That(handler->get_last_event().msg, Is().EqualTo(13));
			for (const auto& handler : unmark_handlers)
				Assert::That(handler->get_last_event().msg, Is().EqualTo(13));
		}
	};
	When(unsubscribe_while_dispatch) {
		TestEvent event = TestEvent {13};
		std::weak_ptr<pisk::os::SysEventHandler<TestEvent>> wptr;
		void SetUp() {
			wptr = external_last_link = std::make_shared<TestEventHandlerWithUnsubscribe>(Root().dispatcher);
			Assert::That(wptr.expired(), Is().EqualTo(false));
			Root().dispatcher->dispatch(event);
		}
		Then(wptr_is_expired) {
			Assert::That(wptr.expired(), Is().EqualTo(true));
		}
	};
};

