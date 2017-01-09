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
#include <pisk/infrastructure/Logger.h>

#include <functional>

using namespace igloo;

struct LogMessage {
	pisk::infrastructure::Logger::Level level;
	std::string tag;
	std::string message;
};

LogMessage lastmessage;

class TestLogStorage : public pisk::infrastructure::LogStorage {

	virtual void store(const pisk::infrastructure::Logger::Level level, const std::string& tag, const std::string& message) noexcept override final {
		lastmessage.level = level;
		lastmessage.tag = tag;
		lastmessage.message = message;
	}
	virtual void store(const pisk::infrastructure::Logger::Level level, const std::string& tag, const std::vector<std::string>& messages) noexcept override final {
		lastmessage.level = level;
		lastmessage.tag = tag;
		lastmessage.message = pisk::utils::algorithm::join(messages, std::string("\n"));
	}
};

Describe(infrastructure_logger) {
	pisk::infrastructure::Logger& logger() {
		return pisk::infrastructure::Logger::get();
	}
	void SetUp() {
		Root().logger().set_log_storage(std::unique_ptr<pisk::infrastructure::LogStorage>());
	}
	Context(log_writter_does_not_set) {
		When(try_to_log) {
			Then(no_exception) {
				using namespace pisk::infrastructure;
				Root().logger().log(Logger::Level::Error, "test", "testmessage");
			}
		};
		When(set_log_writter) {
			void SetUp() {
				Root().logger().set_log_storage(std::make_unique<TestLogStorage>());
			}
			Then(storage_obtain_log_messages)
			{
				using namespace pisk::infrastructure;
				Root().logger().log(Logger::Level::Error, "test", "testmessage");
				Assert::That(lastmessage.level, Is().EqualTo(Logger::Level::Error));
				Assert::That(lastmessage.tag, Is().EqualTo("test"));
				Assert::That(lastmessage.message, Is().EqualTo("testmessage"));
			}
		};
		When(log_formatted) {
			void SetUp() {
				Root().logger().set_log_storage(std::make_unique<TestLogStorage>());
			}
			Then(storage_obtain_foratted_message) {
				using namespace pisk::infrastructure;
				Root().logger().log()
					.add_line("Message: %s", "xxx")
					.add_line("Value: %d", 13)
					.commit(Logger::Level::Information, "itest");
				Assert::That(lastmessage.level, Is().EqualTo(Logger::Level::Information));
				Assert::That(lastmessage.tag, Is().EqualTo("itest"));
				Assert::That(lastmessage.message, Is().EqualTo("Message: xxx\nValue: 13"));
			}
		};
	};
};
