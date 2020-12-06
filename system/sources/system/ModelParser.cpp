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

#include "ModelParser.h"

namespace pisk
{
namespace system
{
	ModelParser::Presentations ModelParser::parse(const utils::property& description) noexcept
	try
	{
		const auto prop_scene_name = description["properties"]["name"];
		const std::string& scene_name = prop_scene_name.is_string() ? prop_scene_name.as_string() : "noname";
		logger::info("modelparser", "Parse object '{}'", scene_name);

		Presentations out;
		for (auto it = description.begin(); it != description.end(); ++it)
			if (it->is_dictionary())
				out[it.get_key()] = *it;
		return out;
	} catch (const infrastructure::Exception&) {
		logger::error("modelparser", "There is an error while loading model");
		return {};
	}
}
}

