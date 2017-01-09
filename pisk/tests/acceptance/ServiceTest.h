// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module pisk of the project pisk.
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


#pragma once

#include <pisk/bdd.h>

#include <pisk/tools/ComponentsLoader.h>
#include <pisk/tools/OsAppInstance.h>
#include <pisk/tools/Application.h>
#include <pisk/tools/MainLoop.h>

#include <future>

template <typename Service>
void InitTestService(const pisk::tools::ServiceRegistry& sl, std::function<std::string (const pisk::tools::InterfacePtr<Service>&)> test, std::string& fail_msg)
{
	auto mainloop = sl.get<pisk::tools::MainLoop>();
	igloo::Assert::That(mainloop, igloo::Is().Not().EqualTo(nullptr));

	auto service = sl.get<Service>();
	if (service == nullptr)
	{
		mainloop->stop();
		fail_msg = "Service not found";
		return;
	}
	igloo::Assert::That(service, igloo::Is().Not().EqualTo(nullptr));

	std::async(std::launch::async, [&fail_msg, test, service, mainloop]() {
		try
		{
			fail_msg = test(service);
		}
		catch (const pisk::infrastructure::Exception&)
		{
			std::cerr << "exception handled" << std::endl;
			fail_msg = "Unhandled exception";
		}
		catch (const std::exception& ex)
		{
			fail_msg = "Unhandled std::exception: " + std::string(ex.what());
		}
		catch (...)
		{
			fail_msg = "Detected unexpected exception";
		}
		mainloop->stop();
	});
}

class TestOsAppInstnace :
	public pisk::tools::OsAppInstance
{
	virtual void release() final override {
		delete this;
	}
};

class TestMainLoop :
	public pisk::tools::MainLoop
{
		virtual void release() final override {
			delete this;
		}
		virtual void run() {}

		virtual void stop() {}
};

inline pisk::tools::OsComponentList make_test_components()
{
	return pisk::tools::OsComponentList {
		{pisk::tools::OsAppInstance::uid, [](const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&) -> pisk::tools::SafeComponentPtr {
			return factory.make<TestOsAppInstnace>();
		}},
		{pisk::tools::MainLoop::uid, [](const pisk::tools::ServiceRegistry&, const pisk::tools::InstanceFactory& factory, const pisk::utils::property&) -> pisk::tools::SafeComponentPtr {
			return factory.make<TestMainLoop>();
		}},
	};
}

extern pisk::infrastructure::ModulePtr CreateModule(const std::string& basename);

template <typename Service>
std::string RunAppForTestService(const pisk::tools::components::DescriptionsList& desc_list, std::function<std::string (const pisk::tools::InterfacePtr<Service>&)> test)
{
	TestOsAppInstnace os_app_instance;

	std::string fail_msg;
	{
		pisk::tools::ApplicationPtr app = pisk::tools::make_application();
		app->run({
			make_test_components(),
			[](const pisk::tools::ServiceRegistry&) -> void { },
			[desc_list](const pisk::tools::ServiceRegistry&) {
				return desc_list;
			},
			[&fail_msg, test](const pisk::tools::ServiceRegistry& sl) {
				InitTestService<Service>(sl, test, fail_msg);
			},
			[](const std::string& basename) -> pisk::infrastructure::ModulePtr {
				return CreateModule(basename);
			}
		});
	}
	return fail_msg;
}

