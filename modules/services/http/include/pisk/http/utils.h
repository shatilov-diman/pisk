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

#include <pisk/defines.h>

#include <pisk/utils/algorithm_utils.h>
#include <pisk/infrastructure/Exception.h>

#include <deque>
#include <regex>
#include <string>
#include <cstring>

#include "structs.h"

namespace pisk
{
namespace services
{
namespace http
{
	inline utils::keystring build_query(const std::deque<std::pair<utils::keystring, utils::keystring>>& content)
	{
		std::deque<std::string> nodes;
		for (const auto& pair : content)
		{
			if (pair.first.empty())
				continue;
			nodes.push_back(pair.first.get_content() + '=' + pair.second.get_content());
		}
		return utils::keystring {
			utils::algorithm::join(nodes, std::string {"&"})
		};
	}
	inline utils::keystring build_domain_path(const URL& url)
	{
		std::string out;
		if (not url.scheme.empty())
			out.append(url.scheme.get_content() + "://");
		if (not url.userinfo.empty())
			out.append(url.userinfo.get_content() + "@");
		out.append(url.host.get_content());
		if (not url.port.empty())
			out.append(":" + url.port.get_content());
		return utils::keystring {out};
	}
	inline utils::keystring build_request_path(const URL& url)
	{
		std::string out = url.path.get_content();
		if (not url.query.empty())
			out.append("?" + url.query.get_content());
		if (not url.fragment.empty())
			out.append("#" + url.fragment.get_content());
		return utils::keystring {out};
	}
	inline utils::keystring to_string(const URL& url)
	{
		const std::string& out = build_domain_path(url).get_content() + build_request_path(url).get_content();
		return utils::keystring {out};
	}
	inline URL to_url(const utils::keystring& url)
	{
		static const char expression[] = "^(([^:\\/?#]+):\\/\\/)?" "(([^@]*)@)?" "([^\\/\\?#:]*)" "(:(\\d*))?" "([^?#]*)?" "(\\?([^#]*))?" "(#(.*))?";
		static const std::regex regex(expression);

		const std::string& content = url.get_content();
		const std::vector<int> indexes {2, 4, 5, 7, 8, 10, 12};
		std::vector<std::string> matches;
		std::copy(
			std::sregex_token_iterator(content.begin(), content.end(), regex, indexes),
			std::sregex_token_iterator(),
			std::back_inserter(matches));

		URL out {
			utils::keystring { matches[0] },
			utils::keystring { matches[1] },
			utils::keystring { matches[2] },
			utils::keystring { matches[3] },
			utils::keystring { matches[4] },
			utils::keystring { matches[5] },
			utils::keystring { matches[6] },
		};
		return out;
	}

namespace headers
{
	inline utils::keystring make_header(const utils::keystring& name, const utils::keystring& value)
	{
		return utils::keystring { name.get_content() + ": " + value.get_content() };
	}
	inline utils::keystring content_length(const std::size_t& length)
	{
		return utils::keystring { "Content-Length: " + std::to_string(length) };
	}
	inline utils::keystring content_type(const utils::keystring& type)
	{
		return utils::keystring { "Content-Type: " + type.get_content() };
	}
}
}
}
}

