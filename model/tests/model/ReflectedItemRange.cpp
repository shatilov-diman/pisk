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

#include <pisk/model/ReflectedObject.h>
#include <pisk/model/ReflectedScene.h>
#include <pisk/model/ReflectedEvent.h>

using namespace igloo;
using namespace pisk::utils;
using namespace pisk::model;

template<typename TObject>
static std::size_t calculate_children_base(TObject& parent) {
	std::size_t count = 0;
	for (auto child : parent.children())
	{
		UNUSED(child);
		++count;
	}
	return count;
}

static std::size_t calculate_children_by_ref(ReflectedObject& parent) {
	return calculate_children_base(parent);
}
static std::size_t calculate_children_by_cref(const ReflectedObject& parent) {
	return calculate_children_base(parent);
}
static std::size_t calculate_const_children_by_ref(ConstReflectedObject& parent) {
	return calculate_children_base(parent);
}
static std::size_t calculate_const_children_by_cref(const ConstReflectedObject& parent) {
	return calculate_children_base(parent);
}

Describe(TestReflectedItemRange) {
	property orig;
	property diff;

	ReflectedObject object{orig, diff};
	const ReflectedObject& cobject = object;

	Spec(empty_object_has_no_children) {
		Assert::That(calculate_children_by_cref(Root().cobject), Is().EqualTo(0U));
	}
	When(add_child) {
		void SetUp() {
			Root().object.child("asd");
		}
		Then(count_of_children_is_one) {
			Assert::That(calculate_children_by_cref(Root().cobject), Is().EqualTo(1U));
		}
	};
	When(add_two_children) {
		void SetUp() {
			Root().object.child("asd");
			Root().object.child("qwe");
		}
		Then(count_of_children_are_two) {
			Assert::That(calculate_children_by_cref(Root().cobject), Is().EqualTo(2U));
		}
		When(remove_once) {
			void SetUp() {
				Root().object.remove_child("qwe");
			}
			Then(count_of_children_are_two) {
				Assert::That(calculate_children_by_cref(Root().cobject), Is().EqualTo(2U));
			}
			Spec(check_diff) {
				property check;
				check["children"]["asd"]["properties"]["id"] = "asd";
				check["children"]["qwe"] = property {};
				Assert::That(Root().diff, Is().EqualTo(check));
			}
		};
	};
};

Context(enumerate) {
	Context(origin) {
		property orig;
		property prop;

		ReflectedObject object{orig, prop};
		const ReflectedObject& cobject = object;

		void SetUp() {
			ReflectedObject filler{property::none_property(), orig};
			filler.child("123");
			filler.child("321");
		}
		Spec(object_has_not_changes) {
			Assert::That(orig.is_none(), Is().EqualTo(false));
			Assert::That(prop.is_none(), Is().EqualTo(true));
			Assert::That(object.has_changes(), Is().EqualTo(false));
			Assert::That(cobject.has_changes(), Is().EqualTo(false));
		}
		Spec(count_in_object_is_two) {
			Assert::That(calculate_children_by_ref(object), Is().EqualTo(0U));
			Assert::That(calculate_children_by_cref(cobject), Is().EqualTo(2U));
		}
		When(try_enumerate_by_non_const_wrapper) {
			Then(its_impossible_to_achieve_original_items) {
				for (auto child : Parent().object.children())
				{
					UNUSED(child);
					Assert::That(!"It's impossible", Is().EqualTo(true));
				}
			}
		};
		Spec(enumerated) {
			for (auto child : object.children())
			{
				UNUSED(child);
				Assert::That(!"It's impossible", Is().EqualTo(true));
			}

			std::size_t count = 0;
			for (auto child : cobject.children())
			{
				Assert::That(child.has_changes(), Is().EqualTo(false));
				Assert::That(child.id().as_string(), Is().EqualTo("123").Or().EqualTo("321"));
				++count;
			}
			Assert::That(count, Is().EqualTo(2U));
		}
	};
	Context(patch) {
		property orig;
		property prop;

		ReflectedObject object{orig, prop};
		const ReflectedObject& cobject = object;

		void SetUp() {
			ReflectedObject filler{property::none_property(), prop};
			filler.child("123");
			filler.child("321");
		}
		Spec(object_has_changes) {
			Assert::That(orig.is_none(), Is().EqualTo(true));
			Assert::That(prop.is_none(), Is().EqualTo(false));
			Assert::That(cobject.has_changes(), Is().EqualTo(true));
		}
		Spec(count_in_object_is_two) {
			Assert::That(calculate_children_by_ref(object), Is().EqualTo(2U));
			Assert::That(calculate_children_by_cref(cobject), Is().EqualTo(2U));
		}
		Spec(enumerated) {
			for (auto child : cobject.children())
			{
				Assert::That(child.has_changes(), Is().EqualTo(true));
				Assert::That(child.id().as_string(), Is().EqualTo("123").Or().EqualTo("321"));
			}
		}
		Spec(check_iterator_get_key) {
			for (auto iter = cobject.children().begin(); iter != cobject.children().end(); ++iter)
			{
				Assert::That(iter.get_key(), Is().EqualTo("123").Or().EqualTo("321"));
				AssertThrowsEx(pisk::utils::PropertyIteratorTypeException, iter.get_index());
			}
		}
	};
	Context(origin_and_patch) {
		property orig;
		property prop;

		ReflectedObject object{orig, prop};
		const ReflectedObject& cobject = object;

		void SetUp() {
			ReflectedObject filler1{property::none_property(), orig};
			filler1.child("123");
			ReflectedObject filler2{property::none_property(), prop};
			filler2.child("321");
		}
		Spec(object_has_changes) {
			Assert::That(orig.is_none(), Is().EqualTo(false));
			Assert::That(prop.is_none(), Is().EqualTo(false));
			Assert::That(cobject.has_changes(), Is().EqualTo(true));
		}
		Spec(count_in_object_is_one) {
			Assert::That(calculate_children_by_ref(object), Is().EqualTo(1U));
			Assert::That(calculate_children_by_cref(cobject), Is().EqualTo(1U));
		}
		Spec(enumerated_is_321) {
			for (auto child : cobject.children())
			{
				Assert::That(child.has_changes(), Is().EqualTo(true));
				Assert::That(child.id().as_string(), Is().EqualTo("321"));
			}
		}
		When(update_state_by_orig) {
			std::vector<std::string> ids;
			void SetUp() {
				for (auto child : Parent().object.origin().children())
				{
					Parent().object.child(child.id().as_keystring()).current_state_id() = "qwe";
					ids.push_back(child.id().as_string());
				}
			}
			Spec(count_in_object_is_two) {
				Assert::That(calculate_children_by_cref(Parent().cobject), Is().EqualTo(2U));
			}
			Then(check_enumeration) {
				Assert::That(Parent().cobject.child("123").current_state_id().as_string(), Is().EqualTo("qwe"));
				Assert::That(Parent().cobject.child("321").current_state_id().is_none(), Is().EqualTo(true));
			}
			Spec(check_iterator_get_key) {
				for (auto iter = Parent().cobject.children().begin(); iter != Parent().cobject.children().end(); ++iter)
				{
					Assert::That(iter.get_key(), Is().EqualTo("123").Or().EqualTo("321"));
					AssertThrowsEx(pisk::utils::PropertyIteratorTypeException, iter.get_index());
				}
			}
		};
	};
};


Context(enumerate_const) {
	Context(origin) {
		property orig;

		ConstReflectedObject object{orig, property::none_property()};
		const ConstReflectedObject& cobject = object;

		void SetUp() {
			ReflectedObject filler{property::none_property(), orig};
			filler.child("123");
			filler.child("321");
		}
		Spec(object_has_not_changes) {
			Assert::That(orig.is_none(), Is().EqualTo(false));
			Assert::That(object.has_changes(), Is().EqualTo(false));
			Assert::That(cobject.has_changes(), Is().EqualTo(false));
		}
		Spec(count_in_object_is_two) {
			Assert::That(calculate_const_children_by_ref(object), Is().EqualTo(2U));
			Assert::That(calculate_const_children_by_cref(cobject), Is().EqualTo(2U));
		}
		Spec(enumerated_is_123) {
			for (auto child : cobject.children())
			{
				Assert::That(child.has_changes(), Is().EqualTo(false));
				Assert::That(child.id().as_string(), Is().EqualTo("123").Or().EqualTo("321"));
			}
		}
		Spec(iterator_get_key_is_123) {
			for (auto iter = cobject.children().begin(); iter != cobject.children().end(); ++iter)
			{
				Assert::That(iter.get_key(), Is().EqualTo("123").Or().EqualTo("321"));
				AssertThrowsEx(pisk::utils::PropertyIteratorTypeException, iter.get_index());
			}
		}
	};
	Context(patch) {
		property prop;

		ConstReflectedObject object{property::none_property(), prop};
		const ConstReflectedObject& cobject = object;

		void SetUp() {
			ReflectedObject filler{property::none_property(), prop};
			filler.child("123");
			filler.child("321");
		}
		Spec(object_has_changes) {
			Assert::That(prop.is_none(), Is().EqualTo(false));
			Assert::That(cobject.has_changes(), Is().EqualTo(true));
		}
		Spec(count_in_object_is_two) {
			Assert::That(calculate_const_children_by_ref(object), Is().EqualTo(2U));
			Assert::That(calculate_const_children_by_cref(cobject), Is().EqualTo(2U));
		}
		Spec(enumerated_is_123) {
			for (auto child : cobject.children())
			{
				Assert::That(child.has_changes(), Is().EqualTo(true));
				Assert::That(child.id().as_string(), Is().EqualTo("123").Or().EqualTo("321"));
			}
		}
	};
	Context(origin_and_patch) {
		property orig;
		property prop;

		ConstReflectedObject object{orig, prop};
		const ConstReflectedObject& cobject = object;

		void SetUp() {
			ReflectedObject filler1{property::none_property(), orig};
			filler1.child("123");
			ReflectedObject filler2{property::none_property(), prop};
			filler2.child("321");
		}
		Spec(object_has_changes) {
			Assert::That(orig.is_none(), Is().EqualTo(false));
			Assert::That(prop.is_none(), Is().EqualTo(false));
			Assert::That(cobject.has_changes(), Is().EqualTo(true));
		}
		Spec(count_in_object_is_one) {
			Assert::That(calculate_const_children_by_ref(object), Is().EqualTo(1U));
			Assert::That(calculate_const_children_by_cref(cobject), Is().EqualTo(1U));
		}
		Spec(enumerated_is_321) {
			for (auto child : cobject.children())
			{
				Assert::That(child.has_changes(), Is().EqualTo(true));
				Assert::That(child.id().as_string(), Is().EqualTo("321"));
			}
		}
	};
};

template<typename TScene>
static std::size_t calculate_events_base(TScene& scene) {
	std::size_t count = 0;
	for (auto event : scene.events())
	{
		UNUSED(event);
		++count;
	}
	return count;
}

static std::size_t calculate_events_by_ref(ReflectedScene& scene) {
	return calculate_events_base(scene);
}
static std::size_t calculate_events_by_cref(const ReflectedScene& scene) {
	return calculate_events_base(scene);
}

Describe(TestReflectedItemRangeByIndex) {
	property orig;
	property diff;

	ReflectedScene scene{orig, diff};
	const ReflectedScene& cscene = scene;

	Spec(empty_scene_has_no_events) {
		Assert::That(calculate_events_by_cref(Root().cscene), Is().EqualTo(0U));
	}
	When(add_event) {
		void SetUp() {
			Root().scene.push_event<ReflectedEvent>().action() = "qwe";
		}
		Then(count_of_events_is_one) {
			Assert::That(calculate_events_by_ref(Root().scene), Is().EqualTo(1U));
		}
	};
	When(add_two_events) {
		void SetUp() {
			Root().scene.push_event<ReflectedEvent>().action() = "qwe";
			Root().scene.push_event<ReflectedEvent>().action() = "asd";
		}
		Then(count_of_events_is_two) {
			Assert::That(calculate_events_by_cref(Root().scene), Is().EqualTo(2U));
		}
		Spec(check_diff) {
			property check;
			check["events"][std::size_t(0)]["action"] = "qwe";
			check["events"][std::size_t(1)]["action"] = "asd";
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		Spec(iterator_get_index) {
			for (auto iter = Root().cscene.events().begin(); iter != Root().cscene.events().end(); ++iter)
			{
				Assert::That(iter.get_index(), Is().EqualTo(std::size_t(0)).Or().EqualTo(std::size_t(1)));
				AssertThrowsEx(pisk::utils::PropertyIteratorTypeException, iter.get_key());
			}
		}
	};
};


