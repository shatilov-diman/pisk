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


#pragma once

#include <pisk/defines.h>

#include <pisk/infrastructure/DataBuffer.h>
#include <pisk/utils/keystring.h>

#include "enum.h"

namespace pisk
{
namespace services
{
namespace http
{
	using Headers = std::vector<utils::keystring>;

	struct URL
	{
		utils::keystring scheme;
		utils::keystring userinfo;
		utils::keystring host;
		utils::keystring port;
		utils::keystring path;
		utils::keystring query;
		utils::keystring fragment;

		bool operator == (const URL& url) const
		{
			return scheme == url.scheme and
				userinfo == url.userinfo and
				host == url.host and
				port == url.port and
				path == url.path and
				query == url.query and
				fragment == url.fragment;
		}
	};

	struct Request
	{
		URL url;
		Headers headers;
		infrastructure::DataBuffer body;
		Method method;
		void* userdata;
	};

	struct Response
	{
		Request request;
		Headers headers;
		infrastructure::DataBuffer body;
		ErrorCode error_code;
		Status status_code;
	};
}
}
}

