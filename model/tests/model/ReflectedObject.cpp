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

using namespace igloo;
using namespace pisk::utils;
using namespace pisk::model;

Describe(TestReflectedObject) {
	property orig;
	property diff;

	ReflectedObject object{orig, diff};
	const ReflectedObject& cobject = object;

	Spec(empty_object) {
		Assert::That(Root().object.has_changes(), Is().EqualTo(false));
		Assert::That(Root().object.is_none(), Is().EqualTo(true));
		Assert::That(Root().object.id().is_none(), Is().EqualTo(true));
	}
	Spec(orig_and_diff_are_none) {
		property check;
		Assert::That(Root().orig, Is().EqualTo(check));
		Assert::That(Root().diff, Is().EqualTo(check));
	}
	Spec(object_from_int) {
		property none;
		Root().orig = 1;
		Root().diff = "qwe";
		AssertThrowsEx(UnexpectedItemTypeException, ReflectedObject(none, Root().diff));
		AssertThrowsEx(UnexpectedItemTypeException, ReflectedObject(Root().orig, none));
		AssertThrowsEx(UnexpectedItemTypeException, ReflectedObject(Root().orig, Root().diff));
	}
	Context(object_fill) {
		void SetUp() {
			Root().object.id() = "qweasd";
			Root().object.current_state_id() = "default";
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["properties"]["id"] = "qweasd";
			check["properties"]["state"] = "default";
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		When(get_id) {
			Then(it_is_qweasd) {
				Assert::That(Root().object.id().is_string(), Is().EqualTo(true));
				Assert::That(Root().object.id().as_string(), Is().EqualTo("qweasd"));
				Assert::That(Root().object.current_state_id().as_string(), Is().EqualTo("default"));
			}
			Then(const_is_qweasd) {
				Assert::That(Root().cobject.id().is_string(), Is().EqualTo(true));
				Assert::That(Root().cobject.id().as_string(), Is().EqualTo("qweasd"));
				Assert::That(Root().cobject.current_state_id().as_string(), Is().EqualTo("default"));
			}
		};
	};
	Context(object_wrong_fill) {
		void SetUp() {
			Root().object.id() = 123;
			Root().object.current_state_id() = 567;
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["properties"]["id"] = 123;
			check["properties"]["state"] = 567;
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		When(get_id) {
			Then(it_is_throw_exception) {
				AssertThrowsEx(UnexpectedItemTypeException, Root().object.id());
				AssertThrowsEx(UnexpectedItemTypeException, Root().object.current_state_id());
			}
			Then(const_is_throw_exception) {
				AssertThrowsEx(UnexpectedItemTypeException, Root().cobject.id());
				AssertThrowsEx(UnexpectedItemTypeException, Root().cobject.current_state_id());
			}
		};
	};
	Context(custom_property) {
		void SetUp() {
			Root().object.property("XXX") = 15;
		}
		Spec(orig_is_none) {
			property check;
			Assert::That(Root().orig, Is().EqualTo(check));
		}
		Spec(check_diff) {
			property check;
			check["properties"]["XXX"] = 15;
			Assert::That(Root().diff, Is().EqualTo(check));
		}
		When(add_custom_property) {
			Then(it_is_available) {
				Assert::That(Root().object.property("XXX").is_int(), Is().EqualTo(true));
				Assert::That(Root().object.property("XXX").as_int(), Is().EqualTo(15));
				Assert::That(Root().cobject.property("XXX").is_int(), Is().EqualTo(true));
				Assert::That(Root().cobject.property("XXX").as_int(), Is().EqualTo(15));
			}
			Then(it_is_changeable) {
				Root().object.property("XXX") = "qwe";
				Assert::That(Root().object.property("XXX").is_string(), Is().EqualTo(true));
				Assert::That(Root().object.property("XXX").as_string(), Is().EqualTo("qwe"));
				Assert::That(Root().cobject.property("XXX").is_string(), Is().EqualTo(true));
				Assert::That(Root().cobject.property("XXX").as_string(), Is().EqualTo("qwe"));
			}
		};
	};
	Context(children) {
		static std::size_t calculate_children(const ConstReflectedObject& parent) {
			std::size_t count = 0;
			for (auto child : parent.children())
			{
				UNUSED(child);
				++count;
			}
			return count;
		}
		Spec(empty_object_has_no_children) {
			Assert::That(calculate_children(Root().cobject), Is().EqualTo(0U));
		}
		Context(non_const_ref) {
			Then(child_created_by_new_id) {
				Assert::That(Root().object.child("qwe").id().as_string(), Is().EqualTo("qwe"));
			}
			Then(child_created_by_empty_id) {
				AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, Root().object.child(""));
			}
			Then(child_created_by_path_id) {
				Assert::That(Root().object.child_by_path("asd/zxc").id().as_string(), Is().EqualTo("zxc"));
				Assert::That(Root().object.child_by_path("asd").id().as_string(), Is().EqualTo("asd"));
				Assert::That(Root().object.child_by_path("qwe").id().as_string(), Is().EqualTo("qwe"));
			}
			Then(child_created_by_empty_path_id) {
				AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, Root().object.child_by_path(""));
			}
		};
		Context(const_ref) {
			Then(child_by_empty_id_is_unavailable) {
				AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, Root().cobject.child(""));
			}
			Then(another_child_is_unavailable) {
				Assert::That(Root().cobject.child("qwe").id().is_none(), Is().EqualTo(true));
			}
			Spec(another_child_by_path_is_unavailable) {
				Assert::That(Root().cobject.child_by_path("asd/zxc").id().is_none(), Is().EqualTo(true));
				Assert::That(Root().cobject.child_by_path("asd").id().is_none(), Is().EqualTo(true));
				Assert::That(Root().cobject.child_by_path("qwe").id().is_none(), Is().EqualTo(true));
			}
			Spec(child_by_empty_path_is_unavailable) {
				AssertThrowsEx(pisk::infrastructure::InvalidArgumentException, Root().cobject.child_by_path(""));
			}
		};
		When(add_child) {
			void SetUp() {
				Root().object.child("asd");
			}
			Spec(check_diff) {
				property check;
				check["children"]["asd"]["properties"]["id"] = "asd";
				Assert::That(Root().diff, Is().EqualTo(check));
			}
			Then(count_of_children_is_one) {
				Assert::That(calculate_children(Root().cobject), Is().EqualTo(1U));
			}
			Then(child_is_available) {
				Assert::That(Root().cobject.child("asd").id().as_string(), Is().EqualTo("asd"));
				Assert::That(Root().object.child("asd").id().as_string(), Is().EqualTo("asd"));
			}
			Then(child_of_child_is_available_by_path) {
				Assert::That(Root().cobject.child_by_path("asd").id(), Is().EqualTo("asd"));
				Assert::That(Root().object.child_by_path("asd").id(), Is().EqualTo("asd"));
			}
			When(remove_child) {
				void SetUp() {
					Root().object.remove_child("asd");
				}
				Spec(check_diff) {
					property check;
					check["children"]["asd"] = property{};
					Assert::That(Root().diff, Is().EqualTo(check));
				}
				Then(count_of_children_is_zero) {
					Assert::That(calculate_children(Root().cobject), Is().EqualTo(1U));
				}
				Then(child_is_not_available) {
					Assert::That(Root().cobject.child("asd").is_none(), Is().EqualTo(true));
				}
			};
			When(add_child_to_child) {
				void SetUp() {
					Root().object.child("asd").child("qwe");
				}
				Spec(check_diff) {
					property check;
					check["children"]["asd"]["properties"]["id"] = "asd";
					check["children"]["asd"]["children"]["qwe"]["properties"]["id"] = "qwe";
					Assert::That(Root().diff, Is().EqualTo(check));
				}
				Then(count_of_children_is_one) {
					Assert::That(calculate_children(Root().cobject.child("asd")), Is().EqualTo(1U));
					Assert::That(calculate_children(Root().cobject), Is().EqualTo(1U));
				}
				Then(child_of_child_is_available_by_path) {
					Assert::That(Root().cobject.child_by_path("asd/qwe").id(), Is().EqualTo("qwe"));
					Assert::That(Root().object.child_by_path("asd/qwe").id(), Is().EqualTo("qwe"));
				}
			};
			When(get_not_existent_child) {
				Then(child_initialized) {
					Assert::That(Root().object.child("zxc").id().as_string(), Is().EqualTo("zxc"));
				}
			};
			When(get_not_existent_child_by_const_ref) {
				Then(none_object_returns) {
					Assert::That(Root().cobject.child("zxc").is_none(), Is().EqualTo(true));
				}
			};
		};
	};
	Context(check_tags) {
		static std::size_t calculate_tags(const ReflectedObject& object) {
			std::size_t count = 0;
			for (auto tag : object.tags())
			{
				UNUSED(tag);
				++count;
			}
			return count;
		}
		static bool object_has_tag(const ReflectedObject& object, const keystring& check_tag) {
			for (auto tag : object.tags())
				if (tag == check_tag)
					return true;
			return false;
		}
		When(no_tags) {
			Then(tags_returns_empty) {
				Assert::That(calculate_tags(Root().object), Is().EqualTo(0U));
			}
			When(add_empty_tag) {
				void SetUp() {
					Root().object.add_tag("");
				}
				Then(tags_returns_three_items) {
					Assert::That(calculate_tags(Root().object), Is().EqualTo(1U));
					Assert::That(object_has_tag(Root().object, ""), Is().EqualTo(true));
				}
				When(remove_empty_tag) {
					void SetUp() {
						Root().object.remove_tag("");
					}
					Then(tags_returns_two_items_again) {
						Assert::That(calculate_tags(Root().object), Is().EqualTo(0U));
						Assert::That(object_has_tag(Root().object, ""), Is().EqualTo(false));
					}
				};
			};
		};
		When(add_tag) {
			void SetUp() {
				Assert::That(object_has_tag(Root().object, "background"), Is().EqualTo(false));
				Root().object.add_tag("background");
			}
			Then(tags_returns_one_item) {
				Assert::That(calculate_tags(Root().object), Is().EqualTo(1U));
				Assert::That(object_has_tag(Root().object, "background"), Is().EqualTo(true));
			}
			When(add_the_tag_again) {
				void SetUp() {
					Root().object.add_tag("background");
				}
				Then(tags_returns_one_item) {
					Assert::That(calculate_tags(Root().object), Is().EqualTo(1U));
					Assert::That(object_has_tag(Root().object, "background"), Is().EqualTo(true));
				}
			};
			When(remove_one_tag) {
				void SetUp() {
					Root().object.remove_tag("background");
				}
				Then(tags_returns_no_items) {
					Assert::That(calculate_tags(Root().object), Is().EqualTo(0U));
					Assert::That(object_has_tag(Root().object, "background"), Is().EqualTo(false));
				}
				When(remove_the_tag_again) {
					void SetUp() {
						Root().object.remove_tag("background");
					}
					Then(tags_returns_no_items) {
						Assert::That(calculate_tags(Root().object), Is().EqualTo(0U));
						Assert::That(object_has_tag(Root().object, "background"), Is().EqualTo(false));
					}
				};
			};
			When(remove_one_nonexisten_tag) {
				void SetUp() {
					Assert::That(object_has_tag(Root().object, "foreground"), Is().EqualTo(false));
					Root().object.remove_tag("foreground");
				}
				Then(tags_returns_one_item) {
					Assert::That(calculate_tags(Root().object), Is().EqualTo(1U));
					Assert::That(object_has_tag(Root().object, "foreground"), Is().EqualTo(false));
				}
			};
		};
		When(add_two_tags) {
			void SetUp() {
				Root().object.add_tag("layer_1");
				Root().object.add_tag("layer_2");
			}
			Then(tags_returns_two_items) {
				Assert::That(calculate_tags(Root().object), Is().EqualTo(2U));
				Assert::That(object_has_tag(Root().object, "layer_1"), Is().EqualTo(true));
				Assert::That(object_has_tag(Root().object, "layer_2"), Is().EqualTo(true));
			}
			When(remove_one_tag) {
				void SetUp() {
					Root().object.remove_tag("layer_1");
				}
				Then(tags_returns_one_item) {
					Assert::That(calculate_tags(Root().object), Is().EqualTo(1U));
					Assert::That(object_has_tag(Root().object, "layer_1"), Is().EqualTo(false));
					Assert::That(object_has_tag(Root().object, "layer_2"), Is().EqualTo(true));
				}
			};
			When(add_empty_tag) {
				void SetUp() {
					Root().object.add_tag("");
				}
				Then(tags_returns_three_items) {
					Assert::That(calculate_tags(Root().object), Is().EqualTo(3U));
					Assert::That(object_has_tag(Root().object, "layer_1"), Is().EqualTo(true));
					Assert::That(object_has_tag(Root().object, "layer_2"), Is().EqualTo(true));
					Assert::That(object_has_tag(Root().object, ""), Is().EqualTo(true));
				}
				When(remove_empty_tag) {
					void SetUp() {
						Root().object.remove_tag("");
					}
					Then(tags_returns_two_items_again) {
						Assert::That(calculate_tags(Root().object), Is().EqualTo(2U));
						Assert::That(object_has_tag(Root().object, "layer_1"), Is().EqualTo(true));
						Assert::That(object_has_tag(Root().object, "layer_2"), Is().EqualTo(true));
						Assert::That(object_has_tag(Root().object, ""), Is().EqualTo(false));
					}
				};
			};
		};
	};
	Context(check_enabled) {
		When(set_enabled) {
			void SetUp() {
				Root().object.enabled() = true;
			}
			Then(is_enabled) {
				Assert::That(Root().object.enabled(), Is().EqualTo(true));
				Assert::That(Root().cobject.enabled(), Is().EqualTo(true));
			}
			Then(check_diff) {
				property check;
				check["properties"]["enabled"] = true;
				Assert::That(Root().diff, Is().EqualTo(check));
			}
		};
		When(set_not_enabled) {
			void SetUp() {
				Root().object.enabled() = false;
			}
			Then(is_not_enabled) {
				Assert::That(Root().object.enabled(), Is().EqualTo(false));
				Assert::That(Root().cobject.enabled(), Is().EqualTo(false));
			}
			Then(check_diff) {
				property check;
				check["properties"]["enabled"] = false;
				Assert::That(Root().diff, Is().EqualTo(check));
			}
		};
		When(load_enabled) {
			void SetUp() {
				Root().diff["properties"]["enabled"] = true;
			}
			Then(is_enabled) {
				Assert::That(Root().object.enabled(), Is().EqualTo(true));
				Assert::That(Root().cobject.enabled(), Is().EqualTo(true));
			}
		};
		When(load_not_enabled) {
			void SetUp() {
				Root().diff["properties"]["enabled"] = false;
			}
			Then(is_not_enabled) {
				Assert::That(Root().object.enabled(), Is().EqualTo(false));
				Assert::That(Root().cobject.enabled(), Is().EqualTo(false));
			}
		};
	};
};

