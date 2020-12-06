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
		logger::info("sceneparser", "Loading scene '{}'", scene_name);

		if (description["objects"].is_array() == false)
		{
			logger::error("sceneparser", "'objects' not found at the scene '{}'", scene_name);
			return {};
		}

		SceneParser::Objects out;
		for (const auto& object : description["objects"])
		{
			const utils::property& model = object["model"];
			const utils::property& overrides = object["overrides"];
			if (model.is_string() == false)
				continue;
			logger::debug("sceneparser", "Detect model '{}'", model.as_string());
			out.emplace_back(std::make_pair(model.as_keystring(), overrides));
		}

		logger::info("sceneparser", "Complete parse scene '{}'", scene_name);
		return out;
	} catch (const infrastructure::Exception&) {
		logger::error("sceneparser", "There is an error while parse scene");
		return {};
	}
}
}
