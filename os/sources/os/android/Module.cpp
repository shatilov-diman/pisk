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
		logger::debug("module", "Pre-load library '{}'", basename);

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
		return pisk::os::impl::Module::load("lib", basename, ".so");
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
	pisk::logger::critical("module", "android assembly does not support direct call CreateModule");
	throw pisk::infrastructure::LogicErrorException();
}

