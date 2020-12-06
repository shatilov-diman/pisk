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


package ltd.charivari.pisk.os;

import android.util.Log;
import android.content.Context;

class OSFacade
{
	public static void LoadNativeLibrary(String library_name)
	{
		Log.i("com.sample.hello_android", "LoadNativeLibrary");
		System.loadLibrary(library_name);
	}
}

