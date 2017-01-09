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


#pragma once

#include <pisk/defines.h>

#include "jni_os_headers.h"
#include "jni_error.h"

namespace pisk
{
namespace os
{
namespace impl
{
	inline void check_result_light(JNIThreadEnv& jni)
	{
		if (jni->ExceptionCheck())
		{
			infrastructure::Logger::get().critical("module", "Java exception occured while process error");
			throw infrastructure::LogicErrorException();
		}
	}
	template <typename Type>
	inline void check_result_light(JNIThreadEnv& jni, Type* jobj)
	{
		check_result_light(jni);
		if (jobj == nullptr)
			throw JNINullPointerException();
	}
	inline void dump_exception_to_log(JNIThreadEnv& jni, jthrowable exception)
	{
		infrastructure::Logger::get().error("module", "Java exception occured:");

		const jclass jLog = jni->FindClass("android/util/Log");
		check_result_light(jni, jLog);

		const jmethodID jLog_getStackTraceString = jni->GetStaticMethodID(jLog, "getStackTraceString", "(Ljava/lang/Throwable;)Ljava/lang/String;");
		check_result_light(jni, jLog_getStackTraceString);

		jobject jobj = jni->CallStaticObjectMethod(jLog, jLog_getStackTraceString, exception);
		check_result_light(jni, jobj);

		const char* str = jni->GetStringUTFChars(static_cast<jstring>(jobj), JNI_FALSE);
		check_result_light(jni);

		infrastructure::Logger::get().info("module", str);

		jni->ReleaseStringUTFChars(static_cast<jstring>(jobj), str);
		check_result_light(jni);
	}

	inline void check_result(JNIThreadEnv& jni)
	{
		if (jni->ExceptionCheck())
		{
			jthrowable exception = jni->ExceptionOccurred();
			jni->ExceptionClear();
			dump_exception_to_log(jni, exception);
			throw JNIErrorException();
		}
	}

	template <typename Type>
	inline void check_result(JNIThreadEnv& jni, Type* jobj)
	{
		check_result(jni);
		if (jobj == nullptr)
			throw JNINullPointerException();
	}
}//namespace impl
}//namespace os
}//namespace pisk

