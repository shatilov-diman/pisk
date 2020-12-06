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


#include "../unix/Module.h"

EXPORT pisk::infrastructure::ModulePtr CreateModule(const std::string& basename)
{
	return pisk::os::impl::Module::load("lib", basename, ".dylib");
}


