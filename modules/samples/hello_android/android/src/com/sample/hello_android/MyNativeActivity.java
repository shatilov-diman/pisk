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


package com.sample.hello_android;

import android.util.Log;

public class MyNativeActivity extends android.app.NativeActivity {
	static {
		Log.i("com.sample.hello_android", "Pre-load native libraries");

		System.loadLibrary("crystax");

		System.loadLibrary("base");
		System.loadLibrary("system");
		System.loadLibrary("os");

		Log.i("com.sample.hello_android", "Pre-load native libraries has complited");
	}
}
