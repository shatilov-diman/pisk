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
#include "ReflectedItemRange.h"
#include "ReflectedProperty.h"
#include "ReflectedPresentation.h"

namespace pisk
{
namespace model
{
	template <typename cv_property>
	class ReflectedObjectBase:
		public ReflectedItemBase<cv_property>
	{
	public:
		ReflectedObjectBase(const cv_property& orig, cv_property& prop):
			ReflectedItemBase<cv_property>(orig, prop)
		{
			if (not (this->is_none() or this->is_dictionary()))
				throw UnexpectedItemTypeException();
		}
		ReflectedObjectBase<const cv_property> const_ref() const
		{
			return ReflectedObjectBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedObjectBase<const cv_property>() const
		{
			return const_ref();
		}

		ReflectedObjectBase<const cv_property> origin() const
		{
			return {this->orig, cv_property::none_property()};
		}

		ReflectedItemBase<cv_property> properties()
		{
			static const utils::keystring kproperties("properties");
			return this->get_dictionary_item(kproperties);
		}
		ReflectedItemBase<const cv_property> properties() const
		{
			return const_ref().property(id);
		}
		ReflectedPropertyBase<cv_property> property(const utils::keystring& id)
		{
			return properties().get_item(id);
		}
		ReflectedItemBase<const cv_property> property(const utils::keystring& id) const
		{
			return const_ref().property(id);
		}

		ReflectedItemBase<cv_property> current_state_id()
		{
			static const utils::keystring kstate("state");
			return properties().get_string_item(kstate);
		}
		ReflectedItemBase<const cv_property> current_state_id() const
		{
			return const_ref().current_state_id();
		}
		ReflectedItemBase<cv_property> id()
		{
			static const utils::keystring kid("id");
			return properties().get_string_item(kid);
		}
		ReflectedItemBase<const cv_property> id() const
		{
			return const_ref().id();
		}

		template <typename Presentation>
		Presentation presentation(const utils::keystring& id)
		{
			static const utils::keystring kpresentations("presentations");
			return this->get_dictionary_item(kpresentations).template get_custom_item<Presentation>(id);
		}
		template <typename Presentation>
		Presentation presentation(const utils::keystring& id) const
		{
			return const_ref().presentation<Presentation>(id);
		}
		template <typename Presentation>
		Presentation presentation()
		{
			static const utils::keystring id = Presentation::presentation_id();
			return this->presentation<Presentation>(id);
		}
		template <typename Presentation>
		Presentation presentation() const
		{
			return const_ref().presentation<Presentation>();
		}

		ReflectedObjectBase<cv_property> child_by_path(const utils::keystring& id_path)
		{
			if (id_path.empty())
				throw infrastructure::InvalidArgumentException();

			utils::keystring tail;
			const utils::keystring& id = path::front(id_path, tail);
			if (tail.empty())
				return child(id);
			return child(id).child_by_path(tail);
		}

		ReflectedObjectBase<const cv_property> child_by_path(const utils::keystring& id_path) const
		{
			return const_ref().child_by_path(id_path);
		}

		template <typename check_type = cv_property, typename = typename std::enable_if<!std::is_const<check_type>::value>::type>
		ReflectedObjectBase<cv_property> child(const utils::keystring& id)
		{
			static const utils::keystring kchildren("children");

			if (id.empty())
				throw infrastructure::InvalidArgumentException();

			auto&& _child = ReflectedObjectBase<cv_property>(this->orig[kchildren][id], this->prop[kchildren][id]);
			_child.id() = id;
			return _child;
		}
		ReflectedObjectBase<const cv_property> child(const utils::keystring& id) const
		{
			static const utils::keystring kchildren("children");

			if (id.empty())
				throw infrastructure::InvalidArgumentException();

			if (not (this->prop[kchildren].is_none() or this->prop[kchildren].is_dictionary()))
				throw UnexpectedItemTypeException();
			if (this->prop[kchildren].contains(id))
				return ReflectedObjectBase<const cv_property>(this->orig[kchildren][id], this->prop[kchildren][id]);
			if (this->orig[kchildren].contains(id))
				return ReflectedObjectBase<const cv_property>(this->orig[kchildren][id], cv_property::none_property());
			return ReflectedObjectBase<const cv_property>(cv_property::none_property(), cv_property::none_property());
		}
		void remove_child(const utils::keystring& id)
		{
			static const utils::keystring kchildren("children");
			if (this->orig[kchildren].contains(id) or this->prop[kchildren].contains(id))
				this->prop[kchildren][id].clear();
		}

		ReflectedItemRange<ReflectedObjectBase<cv_property>, cv_property> children()
		{
			static const utils::keystring kchildren("children");
			return ReflectedItemRange<ReflectedObjectBase<cv_property>, cv_property>(this->orig[kchildren], this->prop[kchildren]);
		}
		ReflectedItemRange<ReflectedObjectBase<const cv_property>, const cv_property> children() const
		{
			return const_ref().children();
		}
	};
	using ConstReflectedObject = ReflectedObjectBase<const utils::property>;
	using ReflectedObject = ReflectedObjectBase<utils::property>;
}
}

