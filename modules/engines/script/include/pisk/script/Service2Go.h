// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module script of the project pisk.
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
#include <pisk/utils/keystring.h>
#include <pisk/utils/property_tree.h>

#include <pisk/tools/ComponentPtr.h>
#include <pisk/script/Script.h>

#include <vector>

namespace pisk
{
namespace script
{
	class HelpProvider
	{
	public:
		virtual utils::keystring help() = 0;

		virtual utils::keystring help(const utils::keystring& keyword) = 0;
	};

	class Service2Go :
		public HelpProvider
	{
	public:
		virtual std::vector<utils::keystring> get_signalers() = 0;

		virtual pisk::utils::auto_unsubscriber subscribe(const utils::keystring& signaler, std::function<void (const Arguments& argumens)>) = 0;

		virtual std::vector<utils::keystring> get_members() = 0;

		virtual Results execute(const utils::keystring& member, const Arguments& arguments) = 0;
	};
	using Service2GoPtr = tools::InterfacePtr<Service2Go>;
}//namespace pisk
}//namespace script

