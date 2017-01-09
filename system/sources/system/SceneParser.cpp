// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module system of the project pisk.
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
#include <pisk/infrastructure/Logger.h>

#include "SceneParser.h"

#include <string>
#include <set>

namespace pisk
{
namespace system
{
	SceneParser::Objects SceneParser::parse(const utils::property& description) noexcept
	try
	{
		const auto prop_scene_name = description["properties"]["name"];
		const std::string& scene_name = prop_scene_name.is_string() ? prop_scene_name.as_string() : "noname";
		infrastructure::Logger::get().info("sceneparser", "Loading scene '%s'", scene_name.c_str());

		if (description["objects"].is_array() == false)
		{
			infrastructure::Logger::get().error("sceneparser", "'objects' not found at the scene '%s'", scene_name.c_str());
			return {};
		}

		SceneParser::Objects out;
		for (const auto& object : description["objects"])
		{
			const utils::property& model = object["model"];
			const utils::property& overrides = object["overrides"];
			if (model.is_string() == false)
				continue;
			infrastructure::Logger::get().debug("sceneparser", "Detect model '%s'", model.as_string().c_str());
			out.emplace_back(std::make_pair(model.as_keystring(), overrides));
		}

		infrastructure::Logger::get().info("sceneparser", "Complete parse scene '%s'", scene_name.c_str());
		return out;
	} catch (const infrastructure::Exception&) {
		infrastructure::Logger::get().error("sceneparser", "There is an error while parse scene");
		return {};
	}
}
}
