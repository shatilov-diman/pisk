// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module base of the project pisk.
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
#include <pisk/utils/json_utils.h>

#include <json/json.h>
#include <json/reader.h>

namespace pisk
{
namespace utils
{
namespace json
{
	Json::Value EXPORT load(const std::string& document) threadsafe noexcept
	{
		Json::Reader reader;
		Json::Value root;
		reader.parse(document, root, false);
		return root;
	}
	Json::Value EXPORT load(const infrastructure::DataBuffer& content) threadsafe noexcept
	{
		std::string document(content.begin(), content.end());
		return load(document);
	}
	Json::Value EXPORT load(const infrastructure::DataStream& data) threadsafe noexcept
	{
		const infrastructure::DataBuffer& content = data.readall();
		return load(content);
	}
	std::string save(const Json::Value& value) threadsafe noexcept
	{
		Json::FastWriter writer;
		writer.omitEndingLineFeed();
		return writer.write(value);
	}

	utils::property convert(const Json::Value& jroot) threadsafe noexcept
	{
		utils::property out;
		switch (jroot.type())
		{
			case Json::nullValue:
			break;
			case Json::intValue:
			case Json::uintValue:
			case Json::realValue:
				out = jroot.asDouble();
			break;
			case Json::stringValue:
				out = jroot.asString();
			break;
			case Json::booleanValue:
				out = jroot.asBool();
			break;
			case Json::objectValue:
				for (const auto& member : jroot.getMemberNames())
					out[member] = convert(jroot[member]);
			break;
			case Json::arrayValue:
				for (Json::ArrayIndex index = 0; index < jroot.size(); ++index)
					out[index] = convert(jroot[index]);
			break;
		};
		return out;
	}
	Json::Value convert(const utils::property& root) threadsafe noexcept
	{
		Json::Value out;
		if (root.is_none()) {
		} else if (root.is_dictionary()) {
			for (auto it = root.begin(); it != root.end(); ++it)
				out[it.get_key().c_str()] = convert(*it);
		} else if (root.is_array()) {
			for (auto it = root.begin(); it != root.end(); ++it)
				out[static_cast<Json::ArrayIndex>(it.get_index())] = convert(*it);
		} else if (root.is_bool()) {
			out = root.as_bool();
		} else if (root.is_int()) {
			out = root.as_int();
		} else if (root.is_long()) {
			out = static_cast<Json::Value::Int64>(root.as_long());
		} else if (root.is_float()) {
			out = root.as_float();
		} else if (root.is_double()) {
			out = root.as_double();
		} else if (root.is_string()) {
			out = root.as_string();
		}
		return out;
	}

	utils::property EXPORT parse_json_to_property(const std::string& document) threadsafe noexcept
	{
		Json::Value jroot = load(document);
		return convert(jroot);
	}
	utils::property EXPORT parse_json_to_property(const infrastructure::DataBuffer& content) threadsafe noexcept
	{
		Json::Value jroot = load(content);
		return convert(jroot);
	}
	utils::property EXPORT parse_json_to_property(const infrastructure::DataStream& data) threadsafe noexcept
	{
		Json::Value jroot = load(data);
		return convert(jroot);
	}

	std::string EXPORT to_string(const utils::property& prop) threadsafe noexcept
	{
		Json::Value jroot = convert(prop);
		return save(jroot);
	}
}
}
}

