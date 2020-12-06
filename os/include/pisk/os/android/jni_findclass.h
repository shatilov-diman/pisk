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


#pragma once

#include <pisk/defines.h>

#include "jni_os_headers.h"
#include "jni_checkresult.h"

namespace pisk
{
namespace os
{
namespace impl
{
	inline jclass jni_findClass(JNIThreadEnv& jni, android_app* application, const std::string& classname)
	{
		jobject jactivity = application->activity->clazz;
		check_result(jni, jactivity);

		const jstring jclassname = jni->NewStringUTF(classname.c_str());
		check_result(jni, jclassname);


		jclass jactivity_class = jni->GetObjectClass(jactivity);
		check_result(jni, jactivity_class);

		jmethodID jgetclassloader_methodid = jni->GetMethodID(jactivity_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
		check_result(jni, jgetclassloader_methodid);

		jobject jclassloader = jni->CallObjectMethod(jactivity, jgetclassloader_methodid);
		check_result(jni, jclassloader);


		jclass jclassloader_class = jni->GetObjectClass(jclassloader);
		check_result(jni, jclassloader_class);

		jmethodID jloadclass_methodid = jni->GetMethodID(jclassloader_class, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
		check_result(jni, jloadclass_methodid);

		jobject jresultclass = jni->CallObjectMethod(jclassloader, jloadclass_methodid, jclassname);
		check_result(jni, jresultclass);

		return static_cast<jclass>(jresultclass);
	}
}//namespace impl
}//namespace os
}//namespace pisk

