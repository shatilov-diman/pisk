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


#pragma once

#include <unordered_set>

namespace pisk
{
namespace utils
{
	template <typename TItem>
	class container
	{
		std::unordered_set<TItem> items;
	public:
		typedef typename std::unordered_set<TItem>::iterator iterator;
		typedef typename std::unordered_set<TItem>::const_iterator const_iterator;
		typedef typename std::unordered_set<TItem>::size_type size_type;

		void push(TItem item) {
			items.emplace(std::move(item));
		}
		void pop(const TItem& item) {
			items.erase(item);
		}
		void clear() {
			items.clear();
		}
		bool empty() const {
			return items.empty();
		}
		size_type size() const {
			return items.size();
		}
		iterator begin() {
			return items.begin();
		}
		const_iterator begin() const {
			return items.begin();
		}
		const_iterator cbegin() const {
			return items.begin();
		}
		iterator end() {
			return items.end();
		}
		const_iterator end() const {
			return items.end();
		}
		const_iterator cend() const {
			return items.cend();
		}
	};
}
}
