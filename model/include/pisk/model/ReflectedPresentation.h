// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module model of the project pisk.
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

#include "ReflectedItem.h"

namespace pisk
{
namespace model
{
	template <typename cv_property = utils::property>
	class ReflectedPresentationBase:
		public ReflectedItemBase<cv_property>
	{
	public:
		ReflectedPresentationBase(const cv_property& orig, cv_property& prop):
			ReflectedItemBase<cv_property>(orig, prop)
		{}
		ReflectedPresentationBase<const cv_property> const_ref() const
		{
			return ReflectedPresentationBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedPresentationBase<const cv_property>() const
		{
			return const_ref();
		}

		template <typename State>
		State state(const utils::keystring& id)
		{
			static const utils::keystring kstates("states");
			return this->get_dictionary_item(kstates).template get_custom_item<State>(id);
		}
		template <typename State>
		State state(const utils::keystring& id) const
		{
			return const_ref().state<State>(id);
		}
	};
	using ConstReflectedPresentation = ReflectedPresentationBase<const utils::property>;
	using ReflectedPresentation = ReflectedPresentationBase<utils::property>;
}
}

