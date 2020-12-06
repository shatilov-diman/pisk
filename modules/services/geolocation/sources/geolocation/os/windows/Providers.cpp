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

#include <pisk/tools/ComponentsLoader.h>

#include <pisk/http/Service.h>
#include <pisk/geolocation/Service.h>

#include "../../ServiceImpl.h"

namespace pisk
{
namespace services
{
namespace geolocation
{
	GeolocationProviderPtr create_webip_geolocation_service(const pisk::services::http::ServicePtr& http_service, const pisk::utils::property& config);

	Providers create_geolocation_providers(const pisk::tools::ServiceRegistry& temp_sl, const pisk::utils::property& config)
	{
		Providers providers;

		auto http_service = temp_sl.get<pisk::services::http::Service>();
		if (http_service != nullptr)
			providers[Provider::IP] = create_webip_geolocation_service(http_service, config);

		return providers;
	}
}
}
}

