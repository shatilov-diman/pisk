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

#include "ServiceImpl.h"

using namespace pisk::tools;
using namespace pisk::services::geolocation;


namespace pisk
{
namespace services
{
namespace geolocation
{
	Providers create_geolocation_providers(const pisk::tools::ServiceRegistry& temp_sl, const pisk::utils::property& config);
}
}
}

ServicePtr CreateServiceForTest(Providers&& providers)
{
	return {{}, std::make_shared<ServiceImpl>(std::move(providers))};
}

SafeComponentPtr __cdecl geolocation_service_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property& config)
{
	static_assert(std::is_convertible<decltype(&geolocation_service_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	return factory.make<ServiceImpl>(create_geolocation_providers(temp_sl, config));
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_geolocation_service_factory()
{
	static_assert(std::is_convertible<decltype(&get_geolocation_service_factory), pisk::tools::components::ComponentFactoryGetter>::value, "Signature was changed!");

	return &geolocation_service_factory;
}

