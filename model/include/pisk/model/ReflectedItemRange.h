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
	template <typename ReflectedItemType, typename cv_property, typename cv_iterator>
	class ReflectedItemIterator :
		public ReflectedItemBase<cv_property>
	{
		cv_iterator iter;
	public:
		ReflectedItemIterator(const cv_property& orig, cv_property& prop, cv_iterator iter):
			ReflectedItemBase<cv_property>(orig, prop),
			iter(iter)
		{}

		explicit operator ReflectedItemType () const
		{
			return **this;
		}

		ReflectedItemType operator*() const
		{
			if (this->orig.is_none() and this->prop.is_none())
				throw infrastructure::OutOfRangeException();

			if (this->prop.is_none())
				return ReflectedItemType(*iter, this->prop);

			if (this->orig.get_type() != this->prop.get_type())
				return ReflectedItemType(cv_property::none_property(), *iter);

			if (this->orig.is_dictionary())
				return ReflectedItemType(this->orig[iter.get_key()], *iter);

			if (this->orig.is_array())
			{
				if (iter.get_index() < this->orig.size())
					return ReflectedItemType(this->orig[iter.get_index()], *iter);
				return ReflectedItemType(cv_property::none_property(), *iter);
			}

			throw infrastructure::LogicErrorException();
		}
		ReflectedItemIterator& operator ++()
		{
			++iter;
			return *this;
		}
		ReflectedItemIterator operator ++(int)
		{
			return ReflectedItemIterator(this->orig, this->prop, ++iter);
		}
		template<typename another_property_iterator>
		bool operator == (const another_property_iterator& it) const
		{
			return iter == it.iter;
		}
		template<typename another_property_iterator>
		bool operator != (const another_property_iterator& it) const
		{
			return !(*this == it);
		}
	};

	template <typename ReflectedItemType, typename cv_property>
	class ReflectedItemRange :
		public ReflectedItemBase<cv_property>
	{
	public:
		using iterator = ReflectedItemIterator<ReflectedItemType, cv_property, typename cv_property::property_iterator>;

		ReflectedItemRange(const cv_property& orig, cv_property& prop):
			ReflectedItemBase<cv_property>(orig, prop)
		{}

		iterator begin() const
		{
			const bool prop_is_collection = this->prop.is_dictionary() or this->prop.is_array();
			const bool has_same_type = this->orig.get_type() == this->prop.get_type();
			if (has_same_type and prop_is_collection)
				return iterator(this->orig, this->prop, this->prop.begin());
			return iterator(cv_property::none_property(), this->prop, this->prop.begin());
		}
		iterator end() const
		{
			return iterator(cv_property::none_property(), this->prop, this->prop.end());
		}
	};

	//TODO: remove specialization: modify ReflectedItemIterator that it contains union of iterators
	template <typename ReflectedItemType>
	class ReflectedItemRange<ReflectedItemType, const utils::property> :
		public ReflectedItemBase<const utils::property>
	{
	public:
		using cv_property = const utils::property;
		using iterator = ReflectedItemIterator<ReflectedItemType, cv_property, typename cv_property::property_const_iterator>;

		ReflectedItemRange(const cv_property& orig, cv_property& prop):
			ReflectedItemBase<cv_property>(orig, prop)
		{}

		iterator begin() const
		{
			if (this->prop.is_none())
				return iterator(this->orig, this->prop, this->orig.begin());

			const bool prop_is_collection = this->prop.is_dictionary() or this->prop.is_array();
			const bool has_same_type = this->orig.get_type() == this->prop.get_type();
			if (has_same_type and prop_is_collection)
				return iterator(this->orig, this->prop, this->prop.begin());
			return iterator(cv_property::none_property(), this->prop, this->prop.begin());
		}
		iterator end() const
		{
			if (this->prop.is_none())
				return iterator(this->orig, this->prop, this->orig.end());

			return iterator(cv_property::none_property(), this->prop, this->prop.end());
		}
	};
}
}

