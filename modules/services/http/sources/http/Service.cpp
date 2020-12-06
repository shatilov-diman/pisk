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

#include <pisk/utils/safequeue.h>
#include <pisk/tools/ComponentsLoader.h>

#include "ServiceImpl.h"
#include "CurlWorker.h"


using namespace pisk::tools;
using namespace pisk::services::http;

SafeComponentPtr __cdecl http_service_factory(const ServiceRegistry&, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&http_service_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto worker = std::make_unique<cURLWorker>();
	return factory.make<ServiceImpl>(std::move(worker));
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_http_service_factory()
{
	static_assert(std::is_convertible<decltype(&get_http_service_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &http_service_factory;
}

