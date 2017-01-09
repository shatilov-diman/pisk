// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module audio of the project pisk.
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


#include "StreamLoader.h"
#include "StreamDecoder.h"

#include <pisk/tools/ComponentsLoader.h>

#include <pisk/system/ResourceManager.h>

namespace pisk
{
namespace audio
{
namespace loaders
{
	static const infrastructure::DataBuffer ogg_signature = {'O', 'g', 'g', 'S'};

	PCMAudioStreamPtr make_ogg_decoder(DataBufferPtr encoded_data);
}
}
}

using namespace pisk::tools;

SafeComponentPtr __cdecl ogg_stream_loader_factory(const ServiceRegistry& temp_sl, const InstanceFactory& factory, const pisk::utils::property&)
{
	static_assert(std::is_convertible<decltype(&ogg_stream_loader_factory), pisk::tools::components::ComponentFactory>::value, "Signature was changed!");

	auto resource_manager = temp_sl.get<pisk::system::ResourceManager>();
	if (resource_manager == nullptr)
		throw pisk::infrastructure::NullPointerException();

	auto resource_loader = factory.make<pisk::audio::loaders::StreamLoader>(
		pisk::audio::loaders::ogg_signature,
		&pisk::audio::loaders::make_ogg_decoder
	);
	resource_manager->get_loader_registry().register_resource_loader(
		pisk::audio::StreamResource::resource_type, resource_loader
	);
	return resource_loader;
}

extern "C"
EXPORT pisk::tools::components::ComponentFactory __cdecl get_ogg_stream_loader_factory()
{
	return &ogg_stream_loader_factory;
}

