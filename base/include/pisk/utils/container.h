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
