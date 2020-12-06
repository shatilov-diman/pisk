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


#include <pisk/infrastructure/Module.h>

#include <string>

extern pisk::infrastructure::ModulePtr CreateModule(const std::string& basename);

int main()
{
	const auto& os_module = CreateModule("os");
	const auto& run_application = os_module->get_procedure<void (*)()>("run_application");
	run_application();
	return 0;
}

