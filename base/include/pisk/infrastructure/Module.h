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
		using ModulePtr = tools::shared_relesable_ptr<Module>;
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

	template <typename Interface, typename InterfacePtr = tools::shared_relesable_ptr<Interface>>
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

		template <typename InputInterface, typename InputInterfacePtr, typename = typename std::enable_if<std::is_convertible<InputInterface*, Interface*>::value>::type>
		ModuleHolderProxy(const ModuleHolderProxy<InputInterface, InputInterfacePtr>& ref) :
			ModuleHolderProxy(ref.template cast<Interface>())
		{}

		ModuleHolderProxy(ModuleHolderProxy&& ref) = default;

		template <typename InputInterface, typename InputInterfacePtr, typename = typename std::enable_if<std::is_convertible<InputInterface*, Interface*>::value>::type>
		ModuleHolderProxy(ModuleHolderProxy<InputInterface, InputInterfacePtr>&& ref) :
			ModuleHolderProxy(ref.template cast_n_move<Interface>())
		{}

		ModuleHolderProxy<Interface, tools::weak_relesable_ptr<Interface>> weak() const
		{
			return {holder, interface};
		}
		template <typename = typename std::enable_if<std::is_convertible<InterfacePtr, tools::weak_relesable_ptr<void>>::value>::type>
		ModuleHolderProxy<Interface, tools::shared_relesable_ptr<Interface>> lock() const
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

		ModuleHolderProxy& operator =(const ModuleHolderProxy<Interface, InterfacePtr>& ref)
		{
			interface = ref.interface;
			holder = ref.holder;
			return *this;
		}

		template <typename InputInterface, typename InputInterfacePtr, typename = typename std::enable_if<std::is_convertible<InputInterface*, Interface*>::value>::type>
		ModuleHolderProxy& operator =(const ModuleHolderProxy<InputInterface, InputInterfacePtr>& ref)
		{
			return *this = ref.template cast<InterfacePtr>();
		}

		ModuleHolderProxy& operator =(ModuleHolderProxy<Interface, InterfacePtr>&& ref)
		{
			interface = std::move(ref.interface);
			holder = std::move(ref.holder);
			return *this;
		}

		template <typename InputInterface, typename InputInterfacePtr, typename = typename std::enable_if<std::is_convertible<InputInterface*, Interface*>::value>::type>
		ModuleHolderProxy& operator =(ModuleHolderProxy<InputInterface, InputInterfacePtr>&& ref)
		{
			return *this = ref.template cast_n_move<InterfacePtr>();
		}

		template <typename RightInterface, typename RightInterfacePtr>
		bool operator ==(const ModuleHolderProxy<RightInterface, RightInterfacePtr>& right) const
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
		template <typename LeftInterface, typename LeftInterfacePtr>
		friend bool operator ==(const ModuleHolderProxy<LeftInterface, LeftInterfacePtr>& left, const ModuleHolderProxy& right)
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
		template <typename RightInterface, typename RightInterfacePtr>
		bool operator <(const ModuleHolderProxy<RightInterface, RightInterfacePtr>& right) const
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

		Interface* get()
		{
			return interface.get();
		}
		const Interface* get() const
		{
			return interface.get();
		}
	};
}
}

