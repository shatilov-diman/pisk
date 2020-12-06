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
#include <pisk/utils/algorithm_utils.h>
#include <pisk/utils/keystring.h>

#include <string>
#include <vector>
#include <functional>

using namespace igloo;
using namespace pisk::utils;

const std::vector<std::string> joindata0;
const std::vector<std::string> joindata1 = {"hello", "world"};

Describe(algorithm_join) {
	When(join_empty) {
		Then(its_empty) {
			std::string result = algorithm::join(joindata0, std::string());
			Assert::That(result, Is().EqualTo(std::string()));
		}
	};
	When(join_hello_and_world_without_linker) {
		Then(its_equal_to_helloworld) {
			std::string result = algorithm::join(joindata1, std::string());
			Assert::That(result, Is().EqualTo(std::string("helloworld")));
		}
	};
	When(join_hello_and_world_with_space) {
		Then(its_equal_to_helloworld) {
			std::string result = algorithm::join(joindata1, std::string(" "));
			Assert::That(result, Is().EqualTo(std::string("hello world")));
		}
	};
	When(join_list) {
		Then(its_joined) {
			std::string result = algorithm::join(joindata1, std::string(" "));
			Assert::That(result, Is().EqualTo(std::string("hello world")));
		}
	};
	When(join_wide_strins) {
		Then(its_joined) {
			std::wstring result = algorithm::join(joindata1, std::wstring(L" "));
			Assert::That(result, Is().EqualTo(std::wstring(L"hello world")));
		}
	};
};

Describe(algorithm_split) {
	Context(no_splitter) {
		When(source_empty) {
			Then(out_is_empty) {
				const auto& result = algorithm::split<std::vector<std::string>>("", "");
				Assert::That(result.empty(), Is().EqualTo(true));
			}
		};
		When(source_not_empty) {
			Then(out_equal_to_source) {
				const auto& result = algorithm::split<std::vector<std::string>>("qweasdzxc", "");
				Assert::That(result.empty(), Is().EqualTo(false));
				Assert::That(result.size(), Is().EqualTo(1U));
				Assert::That(result[0U], Is().EqualTo("qweasdzxc"));
			}
		};
	};
	Context(one_splitter) {
		When(source_empty) {
			Then(out_is_empty) {
				const auto& result = algorithm::split<std::vector<std::string>>("", "/");
				Assert::That(result.empty(), Is().EqualTo(true));
			}
		};
		When(source_without_splitters) {
			Then(out_is_qwe) {
				const auto& result = algorithm::split<std::vector<std::string>>("qwe", "/");
				Assert::That(result.empty(), Is().EqualTo(false));
				Assert::That(result.size(), Is().EqualTo(1U));
				Assert::That(result[0U], Is().EqualTo("qwe"));
			}
		};
		When(source_with_qwe_and_rty) {
			Then(out_is_qwe_and_rty) {
				const auto& result = algorithm::split<std::vector<std::string>>("qwe/rty", "/");
				Assert::That(result.size(), Is().EqualTo(2U));
				Assert::That(result[0U], Is().EqualTo("qwe"));
				Assert::That(result[1U], Is().EqualTo("rty"));
			}
		};
		When(source_with_qwe_and_qwe) {
			Then(out_is_qwe_and_qwe) {
				const auto& result = algorithm::split<std::vector<std::string>>("qwe/qwe", "/");
				Assert::That(result.size(), Is().EqualTo(2U));
				Assert::That(result[0U], Is().EqualTo("qwe"));
				Assert::That(result[1U], Is().EqualTo("qwe"));
			}
		};
		When(source_with_double_splitter) {
			Then(out_is_qwe_and_qwe) {
				const auto& result = algorithm::split<std::vector<std::string>>("qwe//qwe", "/");
				Assert::That(result.size(), Is().EqualTo(2U));
				Assert::That(result[0U], Is().EqualTo("qwe"));
				Assert::That(result[1U], Is().EqualTo("qwe"));
			}
		};
		When(source_with_spliiters_at_front_and_back) {
			Then(out_is_qwe_and_qwe) {
				const auto& result = algorithm::split<std::vector<std::string>>("//qwe//asd//", "/");
				Assert::That(result.size(), Is().EqualTo(2U));
				Assert::That(result[0U], Is().EqualTo("qwe"));
				Assert::That(result[1U], Is().EqualTo("asd"));
			}
		};
		When(source_with_qwe_asd_and_zxc) {
			Then(out_is_qwe_asd_and_zxc) {
				const auto& result = algorithm::split<std::vector<std::string>>("qwe//asd///zxc", "/");
				Assert::That(result.size(), Is().EqualTo(3U));
				Assert::That(result[0U], Is().EqualTo("qwe"));
				Assert::That(result[1U], Is().EqualTo("asd"));
				Assert::That(result[2U], Is().EqualTo("zxc"));
			}
		};
		When(source_with_splitters_only) {
			Then(out_is_qwe_asd_and_zxc) {
				const auto& result = algorithm::split<std::vector<std::string>>("////", "/");
				Assert::That(result.empty(), Is().EqualTo(true));
			}
		};
	};
	Context(two_splitter) {
		When(source_empty) {
			Then(out_is_empty) {
				const auto& result = algorithm::split<std::vector<std::string>>("", "/:");
				Assert::That(result.empty(), Is().EqualTo(true));
			}
		};
		When(source_without_splitters) {
			Then(out_is_qwe) {
				const auto& result = algorithm::split<std::vector<std::string>>("qwe", ":/");
				Assert::That(result.empty(), Is().EqualTo(false));
				Assert::That(result.size(), Is().EqualTo(1U));
				Assert::That(result[0U], Is().EqualTo("qwe"));
			}
		};
		When(source_with_splitters) {
			Then(out_is_qwe_and_qwe) {
				const auto& result = algorithm::split<std::vector<std::string>>("//::qw1//:://qw2:://", ":/");
				Assert::That(result.size(), Is().EqualTo(2U));
				Assert::That(result[0U], Is().EqualTo("qw1"));
				Assert::That(result[1U], Is().EqualTo("qw2"));
			}
		};
		When(source_with_splitters_only) {
			Then(out_is_qwe_and_qwe) {
				const auto& result = algorithm::split<std::vector<std::string>>("//:://:://:://", ":/:/");
				Assert::That(result.empty(), Is().EqualTo(true));
			}
		};
	};
	Context(list_as_container) {
		When(source_empty) {
			Then(out_is_empty) {
				const auto& result = algorithm::split<std::list<std::string>>("", "/:");
				Assert::That(result.empty(), Is().EqualTo(true));
			}
		};
		When(source_without_splitters) {
			Then(out_is_qwe) {
				const auto& result = algorithm::split<std::list<std::string>>("qwe", ":/");
				Assert::That(result.empty(), Is().EqualTo(false));
				Assert::That(result.size(), Is().EqualTo(1U));
			}
		};
		When(source_with_splitters) {
			Then(out_is_qwe_and_qwe) {
				const auto& result = algorithm::split<std::list<std::string>>("//::qw1//:://qw2:://", ":/");
				Assert::That(result.size(), Is().EqualTo(2U));
			}
		};
		When(source_with_splitters_only) {
			Then(out_is_qwe_and_qwe) {
				const auto& result = algorithm::split<std::list<std::string>>("//:://:://:://", ":/:/");
				Assert::That(result.empty(), Is().EqualTo(true));
			}
		};
	};
};

Describe(format_test) {
	When(without_args) {
		Then(its_simple_string) {
			const std::string& result = string::format("simple");
			Assert::That(result, Is().EqualTo("simple"));
		}
	};
	When(int_arg) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", 15);
			Assert::That(result, Is().EqualTo("simple 15 simple"));
		}
	};
	When(char_arg) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", 'q');
			Assert::That(result, Is().EqualTo("simple q simple"));
		}
	};
	When(enum_arg) {
		enum class e {
			x, y, z
		};
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", e::y);
			Assert::That(result, Is().EqualTo("simple 1 simple"));
		}
	};
	When(chars_arg) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", "sss");
			Assert::That(result, Is().EqualTo("simple sss simple"));
		}
	};
	When(float_arg) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", 0.5f);
			Assert::That(result, Is().EqualTo("simple 0.500000 simple"));
		}
	};
	When(std_string_arg) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", std::string("eee"));
			Assert::That(result, Is().EqualTo("simple eee simple"));
		}
	};
	When(std_string_empty_arg) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", std::string{});
			Assert::That(result, Is().EqualTo("simple  simple"));
		}
	};
	When(keystring_arg) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", keystring{"asd"});
			Assert::That(result, Is().EqualTo("simple asd simple"));
		}
	};
	When(keystring_empty_arg) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} simple", keystring{});
			Assert::That(result, Is().EqualTo("simple  simple"));
		}
	};
	When(with_args) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} {} {} {}", "xxx", 15, std::string("qqq"), keystring("asd"));
			Assert::That(result, Is().EqualTo("simple xxx 15 qqq asd"));
		}
	};
	When(with_not_enough_args) {
		Then(its_expected) {
			const std::string& result = string::format("simple {} {} {} {}", "xxx", 15);
			Assert::That(result, Is().EqualTo("simple xxx 15 {} {}"));
		}
	};
	When(with_not_enough_brackets) {
		Then(its_expected) {
			const std::string& result = string::format("simple {}", "xxx", 15, std::string("qqq"), keystring("asd"));
			Assert::That(result, Is().EqualTo("simple xxx"));
		}
	};
	When(with_brackets) {
		Then(its_expected) {
			const std::string& result = string::format("simple }{}{", 42);
			Assert::That(result, Is().EqualTo("simple }42{"));
		}
		Then(its_expected2) {
			const std::string& result = string::format("simple {{}}", 42);
			Assert::That(result, Is().EqualTo("simple {42}"));
		}
	};
};

