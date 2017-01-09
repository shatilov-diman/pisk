// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module system of the project pisk.
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

#include "../../sources/system/PatchPortal.h"

using namespace igloo;
using namespace pisk;


system::PatchPortalPtr CreatePatchPortal();

Describe(PatchDispatcherTest) {
	system::PatchPortalPtr portal = CreatePatchPortal();

	When(make_along_portal) {
		system::PatchGatePtr gate;
		void SetUp() {
			gate = std::move(Root().portal->make_gate());
			Assert::That(gate, Is().Not().EqualTo(nullptr));
		}
		When(pop_from_empty_portal) {
			Then(returns_nullptr) {
				Assert::That(Parent().gate->pop(), Is().EqualTo(nullptr));
			}
		};
		When(push_to_onegate_portal) {
			void SetUp() {
				Parent().gate->push(std::make_shared<system::Patch>("data"));
			}
			Then(portal_still_empty) {
				Assert::That(Parent().gate->pop(), Is().EqualTo(nullptr));
			}
		};
	};
	When(portal_for_two) {
		system::PatchGatePtr gate1;
		system::PatchGatePtr gate2;
		void SetUp() {
			gate1 = std::move(Root().portal->make_gate());
			gate2 = std::move(Root().portal->make_gate());
			Assert::That(gate1, Is().Not().EqualTo(nullptr));
			Assert::That(gate2, Is().Not().EqualTo(nullptr));
		}
		When(pop_from_empty_portal) {
			Then(returns_nullptr) {
				Assert::That(Parent().gate1->pop(), Is().EqualTo(nullptr));
				Assert::That(Parent().gate2->pop(), Is().EqualTo(nullptr));
			}
		};
		When(push_by_first) {
			void SetUp() {
				Parent().gate1->push(std::make_shared<system::Patch>("data"));
			}
			Then(portal_still_empty_for_first) {
				Assert::That(Parent().gate1->pop(), Is().EqualTo(nullptr));
			}
			Then(second_can_pop_data) {
				auto data = Parent().gate2->pop();
				Assert::That(data, Is().Not().EqualTo(nullptr));
				Assert::That(*data, Is().EqualTo(system::Patch("data")));
			}
			When(second_pop_data) {
				void SetUp() {
					Assert::That(Parent().Parent().gate2->pop(), Is().Not().EqualTo(nullptr));
				}
				Then(portal_empty_for_both_again) {
					Assert::That(Parent().Parent().gate1->pop(), Is().EqualTo(nullptr));
					Assert::That(Parent().Parent().gate2->pop(), Is().EqualTo(nullptr));
				}
			};
		};
	};
	When(portal_for_sandwich) {
		system::PatchGatePtr gate1;
		system::PatchGatePtr gate2;
		system::PatchGatePtr gate3;
		void SetUp() {
			gate1 = std::move(Root().portal->make_gate());
			gate2 = std::move(Root().portal->make_gate());
			gate3 = std::move(Root().portal->make_gate());
			Assert::That(gate1, Is().Not().EqualTo(nullptr));
			Assert::That(gate2, Is().Not().EqualTo(nullptr));
			Assert::That(gate3, Is().Not().EqualTo(nullptr));
		}
		When(pop_from_empty_portal) {
			Then(returns_nullptr) {
				Assert::That(Parent().gate1->pop(), Is().EqualTo(nullptr));
				Assert::That(Parent().gate2->pop(), Is().EqualTo(nullptr));
				Assert::That(Parent().gate3->pop(), Is().EqualTo(nullptr));
			}
		};
		When(second_push) {
			void SetUp() {
				Parent().gate2->push(std::make_shared<system::Patch>("data"));
			}
			Then(portal_still_empty_for_second) {
				Assert::That(Parent().gate2->pop(), Is().EqualTo(nullptr));
			}
			Then(first_and_third_can_pop_data) {
				auto data1 = Parent().gate1->pop();
				auto data3 = Parent().gate3->pop();
				Assert::That(data1, Is().Not().EqualTo(nullptr));
				Assert::That(data3, Is().Not().EqualTo(nullptr));
				Assert::That(*data1, Is().EqualTo(system::Patch("data")));
				Assert::That(*data3, Is().EqualTo(system::Patch("data")));
			}
			When(first_and_third_pop_data) {
				void SetUp() {
					Assert::That(Parent().Parent().gate1->pop(), Is().Not().EqualTo(nullptr));
					Assert::That(Parent().Parent().gate3->pop(), Is().Not().EqualTo(nullptr));
				}
				Then(portal_empty_for_again_for_all) {
					Assert::That(Parent().Parent().gate1->pop(), Is().EqualTo(nullptr));
					Assert::That(Parent().Parent().gate2->pop(), Is().EqualTo(nullptr));
					Assert::That(Parent().Parent().gate3->pop(), Is().EqualTo(nullptr));
				}
			};
		};
		When(first_and_third_push) {
			void SetUp() {
				Parent().gate1->push(std::make_shared<system::Patch>("data1"));
				Parent().gate3->push(std::make_shared<system::Patch>("data3"));
			}
			Then(all_in_deal) {
				Assert::That(Parent().gate1->pop(), Is().Not().EqualTo(nullptr));
				Assert::That(Parent().gate2->pop(), Is().Not().EqualTo(nullptr));
				Assert::That(Parent().gate3->pop(), Is().Not().EqualTo(nullptr));
			}
			Then(first_pop_data3) {
				auto data = Parent().gate1->pop();
				Assert::That(data, Is().Not().EqualTo(nullptr));
				Assert::That(*data, Is().EqualTo(system::Patch("data3")));
			}
			Then(third_pop_data1) {
				auto data = Parent().gate3->pop();
				Assert::That(data, Is().Not().EqualTo(nullptr));
				Assert::That(*data, Is().EqualTo(system::Patch("data1")));
			}
			Then(second_pop_both) {
				auto data1 = Parent().gate2->pop();
				auto data3 = Parent().gate2->pop();
				Assert::That(data1, Is().Not().EqualTo(nullptr));
				Assert::That(data3, Is().Not().EqualTo(nullptr));
				Assert::That(*data1, Is().EqualTo(system::Patch("data1")));
				Assert::That(*data3, Is().EqualTo(system::Patch("data3")));
			}
			When(first_and_second_and_third_pop_data) {
				void SetUp() {
					auto data1 = Parent().Parent().gate1->pop();
					auto data2 = Parent().Parent().gate2->pop();
					auto data3 = Parent().Parent().gate3->pop();
					Assert::That(data1, Is().Not().EqualTo(nullptr));
					Assert::That(data2, Is().Not().EqualTo(nullptr));
					Assert::That(data3, Is().Not().EqualTo(nullptr));
					Assert::That(*data1, Is().EqualTo(system::Patch("data3")));
					Assert::That(*data2, Is().EqualTo(system::Patch("data1")));
					Assert::That(*data3, Is().EqualTo(system::Patch("data1")));
				}
				Then(portal_empty_for_first_and_third) {
					Assert::That(Parent().Parent().gate1->pop(), Is().EqualTo(nullptr));
					Assert::That(Parent().Parent().gate3->pop(), Is().EqualTo(nullptr));
				}
				Then(second_can_pop_more) {
					auto data2 = Parent().Parent().gate2->pop();
					Assert::That(data2, Is().Not().EqualTo(nullptr));
					Assert::That(*data2, Is().EqualTo(system::Patch("data3")));
				}
				When(second_pop_again) {
					void SetUp() {
						auto data2 = Parent().Parent().Parent().gate2->pop();
						Assert::That(*data2, Is().EqualTo(system::Patch("data3")));
					}
					Then(portal_empty_for_again_for_all) {
						Assert::That(Parent().Parent().Parent().gate1->pop(), Is().EqualTo(nullptr));
						Assert::That(Parent().Parent().Parent().gate2->pop(), Is().EqualTo(nullptr));
						Assert::That(Parent().Parent().Parent().gate3->pop(), Is().EqualTo(nullptr));
					}
				};
			};
		};
		When(gate2_destroied) {
			void SetUp() {
				Parent().gate2.reset();
			}
			When(gate1_push_data) {
				void SetUp() {
					Parent().Parent().gate1->push(std::make_shared<system::Patch>("data"));
				}
				Then(gate3_can_pop_it) {
					auto data3 = Parent().Parent().gate3->pop();
					Assert::That(data3, Is().Not().EqualTo(nullptr));
					Assert::That(*data3, Is().EqualTo(system::Patch("data")));
				}
			};
		};
	};
};

