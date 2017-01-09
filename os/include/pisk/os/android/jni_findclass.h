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

