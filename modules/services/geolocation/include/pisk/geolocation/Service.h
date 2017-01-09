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


#pragma once

#include <pisk/defines.h>
#include <pisk/utils/signaler.h>
#include <pisk/tools/ComponentPtr.h>

#include <vector>
#include <chrono>

namespace pisk
{
namespace services
{
namespace geolocation
{
	enum class Provider
	{
		None,
		Unknown,
		IP,
		PhoneCell,
		Sattelite,
		Manual,
	};
	enum class ProviderStatus
	{
		None,
		Enabled,
		NotAllowed,
		Disabled,
	};

	struct Location
	{
		std::chrono::system_clock::time_point timestamp;
		double accuracy;
		double latitude;
		double longitude;
		Provider provider;
		bool operator == (const Location& l) const
		{
			return timestamp == l.timestamp and accuracy == l.accuracy and
				longitude == l.longitude and latitude == l.latitude and
				provider == l.provider;
		}
	};
	struct Status
	{
		std::chrono::system_clock::time_point timestamp;
		Provider provider;
		ProviderStatus status;
		bool operator == (const Status& st) const
		{
			return timestamp == st.timestamp and status == st.status and
				provider == st.provider;
		}
	};
	struct Error
	{
		enum class Domain
		{
			None,
			Internal,
			Web,
			Http,
			Service,
		};
		std::chrono::system_clock::time_point timestamp;
		utils::keystring msg;
		Provider provider;
		Domain domain;
		int code;
		bool operator == (const Error& err) const
		{
			return timestamp == err.timestamp and msg == err.msg and
				domain == err.domain and code == err.code and
				provider == err.provider;
		}
	};

	using LocationSignaler = utils::signaler<Location>;
	using StatusSignaler = utils::signaler<Status>;
	using ErrorSignaler = utils::signaler<Error>;

	class Service :
		public core::Component
	{
	public:
		constexpr static const char* uid = "geolocation";

		LocationSignaler on_update_location;
		StatusSignaler on_status_update;
		ErrorSignaler on_update_error;

		virtual std::vector<Provider> get_available_providers() const = 0;

		virtual bool enable_provider(const Provider provider) threadsafe = 0;

		virtual bool disable_provider(const Provider provider) threadsafe = 0;

		virtual Location get_location() const threadsafe = 0;

		virtual Status get_status(const Provider provider) const threadsafe = 0;

		virtual Error get_error(const Provider provider) const threadsafe = 0;
	};
	using ServicePtr = tools::InterfacePtr<Service>;
}
}
}

