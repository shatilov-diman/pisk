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

