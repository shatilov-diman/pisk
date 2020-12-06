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


#pragma once

#include "../defines.h"
#include "../infrastructure/DataStream.h"

#include "property_tree.h"

namespace pisk
{
namespace utils
{
namespace json
{
	utils::property EXPORT parse_json_to_property(const std::string& document) threadsafe noexcept;
	utils::property EXPORT parse_json_to_property(const infrastructure::DataBuffer& content) threadsafe noexcept;
	utils::property EXPORT parse_json_to_property(const infrastructure::DataStream& data) threadsafe noexcept;

	std::string EXPORT to_string(const utils::property& prop) threadsafe noexcept;

	struct JsonToPropertyLoader
	{
		typedef utils::property ParsedType;
		static utils::property parse(const infrastructure::DataStream& data) noexcept
		{
			return parse_json_to_property(data);
		}
	};
}
}
}
