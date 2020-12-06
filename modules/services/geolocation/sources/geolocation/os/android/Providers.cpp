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


#include <pisk/defines.h>

#include <pisk/infrastructure/Logger.h>
#include <pisk/tools/ComponentsLoader.h>

#include <pisk/os/android/jni_error.h>
#include <pisk/os/android/jni_wrapper.h>
#include <pisk/os/android/jni_utils.h>
#include <pisk/os/android/os_app_instance.h>

#include <pisk/geolocation/Service.h>

#include "../../ServiceImpl.h"

namespace pisk
{
namespace services
{
namespace geolocation
{
namespace impl
{
	class JNIGeolocation
	{
		android_app* application;

		jclass geolocation_class;
		jobject geolocation;

	public:
		LocationSignaler on_update_location;
		StatusSignaler on_status_update;
		ErrorSignaler on_update_error;

		explicit JNIGeolocation(android_app* app):
			application(app),
			geolocation_class(nullptr),
			geolocation(nullptr)
		{
			logger::info("geolocation", "jni, JNIGeolocation pre-init, {}", this);
			init_geolocation_director();
			logger::info("geolocation", "jni, JNIGeolocation post-init, {}", this);
		}
		~JNIGeolocation()
		{
			logger::info("geolocation", "jni, JNIGeolocation pre-deinit, {}", this);
			deinit_geolocation_director();
			logger::info("geolocation", "jni, JNIGeolocation post-deinit, {}", this);
		}

	private:
		void init_geolocation_director()
		try
		{
			static const std::string GeolocationDirector_classname = "ltd/charivari/pisk/geolocation/GeolocationDirector";

			os::impl::JNIThreadEnv jni(application);

			auto geolocation_class_local = os::impl::jni_findClass(jni, application, GeolocationDirector_classname);
			check_result(jni, geolocation_class_local);

			geolocation_class = static_cast<jclass>(jni->NewGlobalRef(geolocation_class_local));
			check_result(jni, geolocation_class);

			const jmethodID jmethodid_init = jni->GetMethodID(geolocation_class, "<init>", "(Landroid/content/Context;J)V");
			check_result(jni, jmethodid_init);

			auto geolocation_local = jni->NewObject(geolocation_class, jmethodid_init, application->activity->clazz, reinterpret_cast<long>(this));
			check_result(jni, geolocation_local);

			geolocation = jni->NewGlobalRef(geolocation_local);
			check_result(jni, geolocation);
		}
		catch (...)
		{
			deinit_geolocation_director();
			throw;
		}
		void deinit_geolocation_director()
		try
		{
			os::impl::JNIThreadEnv jni(application);

			detach_native_objectptr();

			if (geolocation != nullptr)
			{
				jni->DeleteGlobalRef(geolocation);
				geolocation = nullptr;
			}

			if (geolocation_class != nullptr)
			{
				jni->DeleteGlobalRef(geolocation_class);
				geolocation_class = nullptr;
			}
		}
		catch (const os::impl::JNIErrorException&)
		{
			logger::warning("geolocation", "jni, exception catched while deinit_geolocation_director. Ignore it.");
		}
		catch (const infrastructure::Exception&)
		{
			logger::critical("geolocation", "jni, exception catched while deinit_geolocation_director. Ignore it.");
		}

		void detach_native_objectptr()
		try
		{
			os::impl::JNIThreadEnv jni(application);

			const jmethodID jmethodid = jni->GetMethodID(geolocation_class, "detach_native_objectptr", "()V");
			check_result(jni, jmethodid);

			jni->CallVoidMethod(geolocation, jmethodid);
			check_result(jni);
		}
		catch (const os::impl::JNIErrorException&)
		{
			logger::error("geolocation", "jni, exception catched while detach_native_objectptr. Ignore it.");
		}
		catch (const infrastructure::Exception&)
		{
			logger::critical("geolocation", "jni, exception catched while detach_native_objectptr. Ignore it.");
		}

	public:
		bool start_fine_locate()
		{
			return exec("enable_fine_locate");
		}
		bool stop_fine_locate()
		{
			return exec("disable_fine_locate");
		}
		bool start_coarse_locate()
		{
			return exec("enable_coarse_locate");
		}
		bool stop_coarse_locate()
		{
			return exec("disable_coarse_locate");
		}

	private:
		bool exec(const std::string& method_name)
		try
		{
			logger::debug("geolocation", "jni, execute '{}'", method_name);

			os::impl::JNIThreadEnv jni(application);

			const jmethodID jmethodid = jni->GetMethodID(geolocation_class, method_name.c_str(), "()Z");
			check_result(jni, jmethodid);

			jboolean result = jni->CallBooleanMethod(geolocation, jmethodid);
			check_result(jni);

			return result != JNI_FALSE;
		}
		catch (const os::impl::JNIErrorException&)
		{
			logger::error("geolocation", "jni, exec catch error while process method '{}'", method_name);
			return false;
		}

	public:
		void jni_on_provider_enabled(const utils::keystring& providername)
		try
		{
			logger::info("geolocation", "jni, on_provider_enabled {}", providername);

			Status status;
			status.timestamp = std::chrono::system_clock::now();
			status.provider = to_provider(providername);
			status.status = ProviderStatus::Enabled;
			this->on_status_update.emit(status);
		}
		catch (const os::impl::JNIErrorException& ex)
		{
			logger::error("geolocation", "jni, jni_on_provider_enabled catch error while process provider '{}'", providername);
		}

		void jni_on_provider_disabled(const utils::keystring& providername)
		try
		{
			logger::info("geolocation", "jni, on_provider_disabled {}", providername);

			Status status;
			status.timestamp = std::chrono::system_clock::now();
			status.provider = to_provider(providername);
			status.status = ProviderStatus::Disabled;
			this->on_status_update.emit(status);
		}
		catch (const os::impl::JNIErrorException& ex)
		{
			logger::error("geolocation", "jni, jni_on_provider_disabled catch error while process provider '{}'", providername);
		}

		void jni_on_location_changed(const utils::keystring& providername, const double accuracy, const double latitude, const double longitude)
		try
		{
			logger::info("geolocation", "jni, on_location_changed {}, accuracy {}, latitude {}, longitude {}",
				providername, accuracy, latitude, longitude
			);

			Location location;
			location.timestamp = std::chrono::system_clock::now();
			location.provider = to_provider(providername);
			location.accuracy = accuracy;
			location.latitude = latitude;
			location.longitude = longitude;
			this->on_update_location.emit(location);
		}
		catch (const os::impl::JNIErrorException& ex)
		{
			logger::error("geolocation", "jni, jni_on_location_changed catch error while process provider '{}'", providername);
		}

	private:
		Provider to_provider(const utils::keystring& providername)
		{
			if (providername == "network")
				return Provider::IP;
			if (providername == "gps")
				return Provider::Sattelite;
			logger::error("geolocation", "jni, unexpected provider name {}", providername);
			return Provider::Unknown;
		}
	};
	using JNIGeolocationPtr = std::shared_ptr<JNIGeolocation>;

	template <Provider provider_type>
	class GeolocationProviderAdaptor final:
		public GeolocationProvider
	{
		JNIGeolocationPtr geolocation;
		utils::auto_unsubscriber update_location_subscription;
		utils::auto_unsubscriber update_status_subscription;
		utils::auto_unsubscriber update_error_subscription;

		virtual bool start_locate() override
		{
			if (geolocation == nullptr)
				return false;
			if (provider_type == Provider::Sattelite)
				return geolocation->start_fine_locate();
			return geolocation->start_coarse_locate();
		}

		virtual bool stop_locate() override
		{
			if (geolocation == nullptr)
				return false;
			if (provider_type == Provider::Sattelite)
				return geolocation->stop_fine_locate();
			return geolocation->stop_coarse_locate();
		}
	public:
		explicit GeolocationProviderAdaptor(JNIGeolocationPtr geolocation):
			geolocation(geolocation)
		{
			if (geolocation == nullptr)
				throw infrastructure::NullPointerException();

			update_location_subscription = subscribe(this->geolocation->on_update_location, this->on_update_location);
			update_status_subscription = subscribe(this->geolocation->on_status_update, this->on_status_update);
			update_error_subscription = subscribe(this->geolocation->on_update_error, this->on_update_error);
		}

	private:
		template<typename Input>
		utils::auto_unsubscriber subscribe(utils::signaler<Input>& signaler, utils::signaler<Input>& forward_signaler)
		{
			return signaler.subscribe([&forward_signaler](const Input& input) {
				if (input.provider == provider_type)
					forward_signaler.emit(input);
			});
		}
	};
}//namespace impl

	Providers create_geolocation_providers(const pisk::tools::ServiceRegistry& temp_sl, const pisk::utils::property& config)
	{
		Providers providers;

		auto app = temp_sl.get<pisk::os::android::OsAppInstance>();
		if (app == nullptr)
		{
			pisk::logger::critical("geolocation", "Unable to locate OsAppInstance");
			throw pisk::infrastructure::NullPointerException();
		}

		auto gelocation = std::make_shared<impl::JNIGeolocation>(app->get_app_instance());

		providers[Provider::IP] = std::make_unique<impl::GeolocationProviderAdaptor<Provider::IP>>(gelocation);
		providers[Provider::Sattelite] = std::make_unique<impl::GeolocationProviderAdaptor<Provider::Sattelite>>(gelocation);

		return providers;
	}
}//namespace geolocation
}//namespace services
}//namespace pisk

extern "C"
JNIEXPORT void JNICALL Java_ltd_charivari_pisk_geolocation_GeolocationDirector_00024LocationListenerImpl_on_1provider_1enabled(
	JNIEnv* jni, jobject obj, const jlong jnativeobjectptr, const jstring jprovider)
{
	using JNIGeolocation = pisk::services::geolocation::impl::JNIGeolocation;

	const pisk::utils::keystring provider_name = pisk::os::impl::jni_to_keystring(jni, jprovider);

	JNIGeolocation* pthis = reinterpret_cast<JNIGeolocation*>(static_cast<long>(jnativeobjectptr));
	pthis->jni_on_provider_enabled(provider_name);
}

extern "C"
JNIEXPORT void JNICALL Java_ltd_charivari_pisk_geolocation_GeolocationDirector_00024LocationListenerImpl_on_1provider_1disabled(
	JNIEnv* jni, jobject obj, const jlong jnativeobjectptr, const jstring jprovider)
{
	using JNIGeolocation = pisk::services::geolocation::impl::JNIGeolocation;

	const pisk::utils::keystring provider_name = pisk::os::impl::jni_to_keystring(jni, jprovider);

	JNIGeolocation* pthis = reinterpret_cast<JNIGeolocation*>(static_cast<long>(jnativeobjectptr));
	pthis->jni_on_provider_disabled(provider_name);
}

extern "C"
JNIEXPORT void JNICALL Java_ltd_charivari_pisk_geolocation_GeolocationDirector_00024LocationListenerImpl_on_1location_1changed(
	JNIEnv* jni, jobject obj, const jlong jnativeobjectptr, const jstring jprovider,
	const jdouble jaccuracy, const jdouble jlatitude, const jdouble jlongitude)
{
	using JNIGeolocation = pisk::services::geolocation::impl::JNIGeolocation;

	const pisk::utils::keystring provider_name = pisk::os::impl::jni_to_keystring(jni, jprovider);

	const double accuracy = static_cast<double>(jaccuracy);
	const double latitude = static_cast<double>(jlatitude);
	const double longitude = static_cast<double>(jlongitude);

	JNIGeolocation* pthis = reinterpret_cast<JNIGeolocation*>(static_cast<long>(jnativeobjectptr));
	pthis->jni_on_location_changed(provider_name, accuracy, latitude, longitude);
}

