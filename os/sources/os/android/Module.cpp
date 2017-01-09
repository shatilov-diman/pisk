// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module os of the project pisk.
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


#include <pisk/infrastructure/Logger.h>
#include <pisk/infrastructure/Module.h>

#include <pisk/os/android/jni_wrapper.h>
#include <pisk/os/android/jni_checkresult.h>
#include <pisk/os/android/jni_findclass.h>

#include "../unix/Module.h"

namespace pisk
{
namespace os
{
namespace impl
{
	static const std::string FacadeClassname = "ltd/charivari/pisk/os/OSFacade";

	void jni_loadLibrary(android_app* application, const std::string& basename)
	{
		infrastructure::Logger::get().debug("module", "Pre-load library '%s'", basename.c_str());

		JNIThreadEnv jni(application);

		const jclass jclazz = jni_findClass(jni, application, FacadeClassname);
		check_result(jni, jclazz);

		const jmethodID jLoadNativeLibrary = jni->GetStaticMethodID(jclazz, "LoadNativeLibrary", "(Ljava/lang/String;)V");
		check_result(jni, jLoadNativeLibrary);

		const jstring jbasename = jni->NewStringUTF(basename.c_str());
		check_result(jni, jbasename);

		jni->CallStaticVoidMethod(jclazz, jLoadNativeLibrary, jbasename);
		check_result(jni);
	}

	pisk::infrastructure::ModulePtr create_module(const std::string& basename, android_app* application)
	try
	{
		pisk::os::impl::jni_loadLibrary(application, basename);
		return pisk::os::impl::Module::load("", basename);
	}
	catch (const pisk::os::impl::JNIErrorException&)
	{
		return nullptr;
	}
}
}
}


EXPORT pisk::infrastructure::ModulePtr CreateModule(const std::string&)
{
	pisk::infrastructure::Logger::get().critical("module", "android assembly does not support direct call CreateModule");
	throw pisk::infrastructure::LogicErrorException();
}

