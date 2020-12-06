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

