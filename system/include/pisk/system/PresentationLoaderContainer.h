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


#pragma once

#include <pisk/defines.h>
#include <pisk/utils/noncopyable.h>
#include <pisk/utils/property_tree.h>
#include <pisk/core/Object.h>

#include "ResourceManager.h"

#include <memory>
#include <vector>

namespace pisk
{
namespace system
{
	class PresentationLoader : public utils::noncopyable
	{
	public:
		virtual core::PresentationPtr load(const core::ObjectPtr& object, const utils::property& description, const system::ResourceManager& resources_manager) const noexcept = 0;
	};
	typedef std::unique_ptr<PresentationLoader> PresentationLoaderPtr;

	class PresentationLoaderContainer : public utils::noncopyable
	{
		std::map<utils::keystring, PresentationLoaderPtr> presentationloader;
	public:
		void set(const utils::keystring& engine, PresentationLoaderPtr&& loader) {
			if (loader == nullptr)
				throw infrastructure::NullPointerException();
			presentationloader[engine].swap(loader);
			if (loader != nullptr)
				throw infrastructure::LogicErrorException();
		}
		void remove(const utils::keystring& engine) {
			presentationloader.erase(engine);
		}
		PresentationLoader& get(const utils::keystring& engine) const
		try {
			return *presentationloader.at(engine);
		} catch (const std::out_of_range&) {
			throw infrastructure::OutOfRangeException();
		}
	};

}
}
