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

#include "../defines.h"
#include "../utils/noncopyable.h"
#include "../tools/Releasable.h"

#include <type_traits>
#include <memory>

namespace pisk
{
namespace infrastructure
{
	namespace impl
	{
		class Module;
	}

	class Module :
		public tools::Releasable,
		public utils::noncopyable
	{
	public:
		using ModulePtr = std::shared_ptr<Module>;
		using ProcedurePtr = void (*)(void);

		virtual ProcedurePtr find_procedure(const std::string& name) = 0;

		template <typename CallablePtr>
		std::function<typename std::remove_pointer<CallablePtr>::type> get_procedure(const std::string& name)
		{
			static_assert(std::is_function<typename std::remove_pointer<CallablePtr>::type>::value, "Module::get_procedure expected function pointeres only");

			ProcedurePtr procedure = find_procedure(name);
			return reinterpret_cast<CallablePtr>(procedure);
		}
	};
	typedef Module::ModulePtr ModulePtr;

	/* For clean all actions should processed in next order: first interface, second holder */

	template <typename Interface, typename InterfacePtr = std::shared_ptr<Interface>>
	class ModuleHolderProxy
	{
		InterfacePtr interface;
		ModulePtr holder;

	public:
		ModuleHolderProxy() = default;
		ModuleHolderProxy(ModulePtr holder, InterfacePtr interface) :
			interface(interface),
			holder(holder)
		{}
		~ModuleHolderProxy()
		{
			interface.reset();
			holder.reset();
		}

		ModuleHolderProxy(const ModuleHolderProxy& ref) = default;

		template <typename InputInterface, typename = typename std::enable_if<std::is_convertible<InputInterface*, Interface*>::value>::type>
		ModuleHolderProxy(const ModuleHolderProxy<InputInterface>& ref) :
			ModuleHolderProxy(ref.template cast<Interface>())
		{}

		ModuleHolderProxy(ModuleHolderProxy&& ref) = default;

		template <typename InputInterface, typename = typename std::enable_if<std::is_convertible<InputInterface*, Interface*>::value>::type>
		ModuleHolderProxy(ModuleHolderProxy<InputInterface>&& ref) :
			ModuleHolderProxy(ref.template cast_n_move<Interface>())
		{}

		ModuleHolderProxy<Interface, std::weak_ptr<Interface>> weak() const
		{
			return {holder, interface};
		}
		template <typename = typename std::enable_if<std::is_convertible<InterfacePtr, std::weak_ptr<void>>::value>::type>
		ModuleHolderProxy<Interface, std::shared_ptr<Interface>> lock() const
		{
			return {holder, interface.lock()};
		}

		template<typename OutInterface>
		ModuleHolderProxy<OutInterface> cast() const
		{
			auto outinterface = std::dynamic_pointer_cast<OutInterface>(interface);
			return {holder, outinterface};
		}
		template<typename OutInterface>
		ModuleHolderProxy<OutInterface> cast_n_move()
		{
			auto outinterface = std::dynamic_pointer_cast<OutInterface>(interface);
			interface.reset();
			return {std::move(holder), outinterface};
		}
		void reset()
		{
			ModuleHolderProxy X(std::move(*this));
		}

		ModuleHolderProxy& operator =(const ModuleHolderProxy<Interface>& ref)
		{
			interface = ref.interface;
			holder = ref.holder;
			return *this;
		}

		template <typename InputInterface, typename = typename std::enable_if<std::is_convertible<InputInterface*, Interface*>::value>::type>
		ModuleHolderProxy& operator =(const ModuleHolderProxy<InputInterface>& ref)
		{
			return *this = ref.template cast<InterfacePtr>();
		}

		ModuleHolderProxy& operator =(ModuleHolderProxy<Interface>&& ref)
		{
			interface = std::move(ref.interface);
			holder = std::move(ref.holder);
			return *this;
		}

		template <typename InputInterface, typename = typename std::enable_if<std::is_convertible<InputInterface*, Interface*>::value>::type>
		ModuleHolderProxy& operator =(ModuleHolderProxy<InputInterface>&& ref)
		{
			return *this = ref.template cast_n_move<InterfacePtr>();
		}

		template <typename RightInterface>
		bool operator ==(const ModuleHolderProxy<RightInterface>& right) const
		{
			return interface == right.interface;
		}
		bool operator ==(const std::nullptr_t& right) const
		{
			return interface == right;
		}
		friend bool operator ==(const std::nullptr_t& left, const ModuleHolderProxy& right)
		{
			return left == right.interface;
		}
		template <typename InterfaceLeft>
		friend bool operator ==(const ModuleHolderProxy<InterfaceLeft>& left, const ModuleHolderProxy& right)
		{
			return left.interface == right.interface;
		}

		template <typename Right>
		bool operator !=(const Right& right) const
		{
			return !(*this == right);
		}
		template <typename Left>
		friend bool operator !=(const Left& left, const ModuleHolderProxy& right)
		{
			return !(left == right);
		}

		bool operator <(const std::nullptr_t& right) const
		{
			return interface < right;
		}
		template <typename RightInterface>
		bool operator <(const ModuleHolderProxy<RightInterface>& right) const
		{
			return interface < right.interface;
		}

		explicit operator bool () const
		{
			return static_cast<bool>(interface);
		}

		Interface& operator *()
		{
			return *interface;
		}
		const Interface& operator *() const
		{
			return *interface;
		}
		InterfacePtr operator ->()
		{
			return interface;
		}
		const InterfacePtr operator ->() const
		{
			return interface;
		}
	};
}
}

