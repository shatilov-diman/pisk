// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module http of the project pisk.
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
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

