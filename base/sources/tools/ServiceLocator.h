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

#include <pisk/defines.h>

#include <pisk/utils/noncopyable.h>
#include <pisk/utils/keystring.h>

#include <pisk/infrastructure/Exception.h>

#include <pisk/tools/Releasable.h>
#include <pisk/tools/ComponentPtr.h>

#include <type_traits>
#include <map>

namespace pisk
{
namespace tools
{
namespace details
{
	class DuplicateFoundException : public infrastructure::Exception
	{};

	//TODO: replace with specialization!
	template <typename BaseInterfaceHolder>
	struct DefaultTraits
	{
		template <typename Interface>
		using InterfacePtr = pisk::tools::InterfacePtr<Interface>;

		using UID = utils::keystring;

		template <typename Interface>
		static UID generate_uid()
		{
			return Interface::uid;
		}

		template <typename Interface>
		static InterfacePtr<Interface> extract(const BaseInterfaceHolder& holder)
		{
			return holder.template cast<Interface>();
		}
	};
}

template <typename InstanceHolder, typename Traits = details::DefaultTraits<InstanceHolder>>
class ServiceLocator :
	public utils::noncopyable
{
	std::map<typename Traits::UID, InstanceHolder> components;

public:
	template<typename Interface>
	using InterfacePtr = typename Traits::template InterfacePtr<Interface>;

	using UID = typename Traits::UID;

	void add(const UID& uid, const InstanceHolder& item)
	{
		if (components.find(uid) != components.end())
			throw details::DuplicateFoundException();
		replace(uid, item);
	}
	void replace(const UID& uid, const InstanceHolder& item)
	{
		components[uid] = item;
	}
	void remove(const UID& uid)
	{
		components.erase(uid);
	}
	InstanceHolder get(const UID& uid) const
	{
		auto find = components.find(uid);
		if (find == components.end())
			return {};
		return find->second;
	}
	template<typename Interface>
	typename Traits::template InterfacePtr<Interface> get(const UID& uid) const
	{
		auto find = components.find(uid);
		if (find == components.end())
			return {};
		return Traits::template extract<Interface>(find->second);
	}
	template<typename Interface>
	typename Traits::template InterfacePtr<Interface> get() const
	{
		return get<Interface>(Traits::template generate_uid<Interface>());
	}
	void clear()
	{
		components.clear();
	}
};
using ServiceLocatorPtr = std::unique_ptr<ServiceLocator<SafeComponentPtr>>;

}
}

