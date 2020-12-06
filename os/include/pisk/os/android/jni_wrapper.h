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

#include <pisk/infrastructure/Logger.h>

#include "jni_os_headers.h"
#include "jni_error.h"

namespace pisk
{
namespace os
{
namespace impl
{
	class JNIThreadEnv
	{
		android_app* application;
		JNIEnv* jni;

		bool own_attach;

	public:
		explicit JNIThreadEnv(android_app* _application):
			application(_application),
			jni(nullptr),
			own_attach(false)
		{
			if (application == nullptr)
				throw infrastructure::NullPointerException();

			if (not find_jni())
				attach();
			if (jni == nullptr)
				throw infrastructure::NullPointerException();

			push_frame();
		}
		~JNIThreadEnv()
		{
			pop_frame();
			detach();
		}

		JNIEnv* operator -> () const
		{
			return jni;
		}
		operator JNIEnv* () const
		{
			return jni;
		}

	private:
		bool find_jni()
		{
			if (jni != nullptr)
				throw infrastructure::NullPointerException();
			const auto result = application->activity->vm->GetEnv(reinterpret_cast<void**>(&jni), JNI_VERSION_1_1);
			if (result == JNI_OK)
				return true;
			if (result == JNI_EDETACHED)
				return false;
			logger::critical("module", "JNI 1.1 is not supported");
			throw infrastructure::InitializeError();
		}
		void attach()
		{
			if (own_attach)
				throw infrastructure::LogicErrorException();
			if (jni != nullptr)
				throw infrastructure::NullPointerException();
			const auto result = application->activity->vm->AttachCurrentThread(&jni, nullptr);
			if (jni == nullptr)
				throw infrastructure::NullPointerException();
			own_attach = true;
		}
		void detach()
		{
			if (jni == nullptr)
				throw infrastructure::NullPointerException();
			jni = nullptr;
			if (not own_attach)
				return;
			application->activity->vm->DetachCurrentThread();
			own_attach = false;
		}
		void push_frame()
		{
			if (jni == nullptr)
				throw infrastructure::NullPointerException();
			const jint result = jni->PushLocalFrame(6);
			if (result < 0)
				throw infrastructure::OutOfMemoryException();
		}
		void pop_frame()
		{
			if (jni == nullptr)
				throw infrastructure::NullPointerException();
			jni->PopLocalFrame(nullptr);
		}
	};
}//namespace impl
}//namespace os
}//namespace pisk

