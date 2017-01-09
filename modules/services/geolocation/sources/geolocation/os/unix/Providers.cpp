// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module geolocation of the project pisk.
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

