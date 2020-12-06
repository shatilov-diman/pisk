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
#include <pisk/infrastructure/Logger.h>

#include <functional>

using namespace igloo;

struct LogMessage {
	pisk::infrastructure::Logger::Level level;
	std::string tag;
	std::string message;
};

LogMessage lastmessage;

using logger = pisk::infrastructure::Logger;

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
	void SetUp() {
		logger::set_log_storage(std::unique_ptr<pisk::infrastructure::LogStorage>());
	}
	Context(log_writter_does_not_set) {
		When(try_to_log) {
			Then(no_exception) {
				using namespace pisk::infrastructure;
				logger::log(Logger::Level::Error, "test", "testmessage");
			}
		};
		When(set_log_writter) {
			void SetUp() {
				logger::set_log_storage(std::make_unique<TestLogStorage>());
			}
			Then(storage_obtain_log_messages)
			{
				using namespace pisk::infrastructure;
				logger::log(Logger::Level::Error, "test", "testmessage");
				Assert::That(lastmessage.level, Is().EqualTo(Logger::Level::Error));
				Assert::That(lastmessage.tag, Is().EqualTo("test"));
				Assert::That(lastmessage.message, Is().EqualTo("testmessage"));
			}
		};
		When(log_formatted) {
			void SetUp() {
				logger::set_log_storage(std::make_unique<TestLogStorage>());
			}
			Then(storage_obtain_foratted_message) {
				using namespace pisk::infrastructure;
				logger::log()
					.add_line("Message: {}", "xxx")
					.add_line("Value: {}", 13)
					.commit(Logger::Level::Information, "itest");
				Assert::That(lastmessage.level, Is().EqualTo(Logger::Level::Information));
				Assert::That(lastmessage.tag, Is().EqualTo("itest"));
				Assert::That(lastmessage.message, Is().EqualTo("Message: xxx\nValue: 13"));
			}
		};
	};
};

