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


#include <pisk/defines.h>

#include <pisk/system/ResourcePackManager.h>

#include <algorithm>
#include <memory>
#include <vector>
#include <set>

namespace pisk
{
namespace system
{
namespace impl
{
	class PackManager :
		public system::ResourcePackManager
	{
		using node = std::pair<std::string, pisk::system::ResourcePackPtr>;
		std::vector<node> packs;

		virtual void set_pack(const std::string& pack_name, ResourcePackPtr&& pack) noexcept override final
		{
			auto found = std::find_if(packs.begin(), packs.end(), [&pack_name](node& item) {return item.first == pack_name;});
			if (found != packs.end())
				std::swap(found->second, pack);
			else
				packs.emplace_back(std::make_pair(pack_name, std::move(pack)));
		}
		virtual void remove_pack(const std::string& pack_name) noexcept override final
		{
			auto newend = std::remove_if(packs.begin(), packs.end(), [&pack_name](node& item) {return item.first == pack_name;});
			packs.erase(newend, packs.end());
		}
		virtual void clear() noexcept override final
		{
			packs.clear();
		}
	public:
		infrastructure::DataStreamPtr open(const std::string& rid) threadsafe const
		{
			for (const auto& pack : utils::iterators::backwards(packs))
			{
				infrastructure::DataStreamPtr res = pack.second->open(rid);
				if (res)
					return res;
			}
			throw ResourceNotFound();
		}
	};
}
}
}

