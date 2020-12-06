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

#include <pisk/utils/keystring.h>
#include <pisk/infrastructure/Exception.h>

#include "jni_os_headers.h"
#include "jni_checkresult.h"
#include "jni_findclass.h"

namespace pisk
{
namespace os
{
namespace impl
{
	inline utils::keystring jni_to_keystring(JNIEnv* jni, jstring jstr)
	{
		if (jni == nullptr)
			throw infrastructure::NullPointerException();

		jboolean isCopy = JNI_FALSE;
		const char* str = jni->GetStringUTFChars(jstr, &isCopy);

		const pisk::utils::keystring out {str};

		jni->ReleaseStringUTFChars(jstr, str);
		return out;
	}
}//namespace impl
}//namespace os
}//namespace pisk

