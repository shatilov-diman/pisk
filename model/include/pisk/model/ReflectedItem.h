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

#include <pisk/utils/noncopyable.h>
#include <pisk/utils/property_tree.h>

#include <pisk/infrastructure/Exception.h>

#include <vector>

namespace pisk
{
namespace model
{
	class UnexpectedItemTypeException :
		public infrastructure::Exception
	{};

	template <typename cv_property>
	class ReflectedItemBase
	{
	protected:
		const cv_property& orig;
		cv_property& prop;

	public:
		ReflectedItemBase(const cv_property& orig, cv_property& prop):
			orig(orig),
			prop(prop)
		{}
		ReflectedItemBase<const cv_property> const_ref() const
		{
			return ReflectedItemBase<const cv_property>(this->orig, this->prop);
		}
		operator ReflectedItemBase<const cv_property>() const
		{
			return const_ref();
		}

		template <typename Right>
		ReflectedItemBase& operator = (const Right& right)
		{
			prop = right;
			return *this;
		}

		ReflectedItemBase<const cv_property> origin() const
		{
			return {orig, cv_property::none_property()};
		}

		void clear()
		{
			prop.clear();
		}

		bool has_changes() const
		{
			return not prop.is_none();
		}

		bool has_origin() const
		{
			return not orig.is_none();
		}

		bool is_changed() const
		{
			return prop != orig;
		}

		bool is_none() const
		{
			return prop.is_none() and orig.is_none();
		}

		bool is_bool() const
		{
			if (not prop.is_none())
				return prop.is_bool();
			return orig.is_bool();
		}
		bool as_bool() const
		{
			if (not prop.is_none())
				return prop.as_bool();
			return orig.as_bool();
		}

		bool is_int() const
		{
			if (not prop.is_none())
				return prop.is_int();
			return orig.is_int();
		}
		int as_int() const
		{
			if (not prop.is_none())
				return prop.as_int();
			return orig.as_int();
		}
		bool is_long() const
		{
			if (not prop.is_none())
				return prop.is_long();
			return orig.is_long();
		}
		long as_long() const
		{
			if (not prop.is_none())
				return prop.as_long();
			return orig.as_long();
		}
		bool is_float() const
		{
			if (not prop.is_none())
				return prop.is_float();
			return orig.is_float();
		}
		float as_float() const
		{
			if (not prop.is_none())
				return prop.as_float();
			return orig.as_float();
		}
		bool is_double() const
		{
			if (not prop.is_none())
				return prop.is_double();
			return orig.is_double();
		}
		double as_double() const
		{
			if (not prop.is_none())
				return prop.as_double();
			return orig.as_double();
		}

		bool is_string() const
		{
			if (not prop.is_none())
				return prop.is_string();
			return orig.is_string();
		}
		std::string as_string() const
		{
			if (not prop.is_none())
				return prop.as_string();
			return orig.as_string();
		}
		utils::keystring as_keystring() const
		{
			if (not prop.is_none())
				return prop.as_keystring();
			return orig.as_keystring();
		}

		bool is_dictionary() const
		{
			if (not prop.is_none())
				return prop.is_dictionary();
			return orig.is_dictionary();
		}

		bool is_array() const
		{
			if (not prop.is_none())
				return prop.is_array();
			return orig.is_array();
		}

	public:
		bool operator == (const bool value) const
		{
			if (not prop.is_none()) {
				if (prop.is_bool())
					return prop.as_bool() == value;
			} else if (not orig.is_none()) {
				if (orig.is_bool())
					return orig.as_bool() == value;
			}
			return false;
		}
		bool operator == (const int value) const
		{
			if (not prop.is_none()) {
				if (prop.is_int())
					return prop.as_int() == value;
			} else if (not orig.is_none()) {
				if (orig.is_int())
					return orig.as_int() == value;
			}
			return false;
		}
		bool operator == (const long value) const
		{
			if (not prop.is_none()) {
				if (prop.is_long())
					return prop.as_long() == value;
			} else if (not orig.is_none()) {
				if (orig.is_long())
					return orig.as_long() == value;
			}
			return false;
		}
		bool operator == (const float value) const
		{
			if (not prop.is_none()) {
				if (prop.is_float())
					return prop.as_float() == value;
			} else if (not orig.is_none()) {
				if (orig.is_float())
					return orig.as_float() == value;
			}
			return false;
		}
		bool operator == (const double value) const
		{
			if (not prop.is_none()) {
				if (prop.is_double())
					return prop.as_double() == value;
			} else if (not orig.is_none()) {
				if (orig.is_double())
					return orig.as_double() == value;
			}
			return false;
		}
		bool operator == (const utils::keystring& value) const
		{
			if (not prop.is_none()) {
				if (prop.is_string())
					return prop.as_keystring() == value;
			} else if (not orig.is_none()) {
				if (orig.is_string())
					return orig.as_keystring() == value;
			}
			return false;
		}
		bool operator == (const std::string& value) const
		{
			if (not prop.is_none()) {
				if (prop.is_string())
					return prop.as_string() == value;
			} else if (not orig.is_none()) {
				if (orig.is_string())
					return orig.as_string() == value;
			}
			return false;
		}
		bool operator == (const char* value) const
		{
			if (not prop.is_none()) {
				if (prop.is_string())
					return prop.as_string() == value;
			} else if (not orig.is_none()) {
				if (orig.is_string())
					return orig.as_string() == value;
			}
			return false;
		}

		template <typename Left>
		friend bool operator == (const Left& left, const ReflectedItemBase<cv_property>& right)
		{
			return (right == left);
		}

		template <typename Right>
		bool operator != (const Right& right)
		{
			return not (*this == right);
		}

		template <typename Left>
		friend bool operator != (const Left& left, const ReflectedItemBase<cv_property>& right)
		{
			return (right != left);
		}

	public:
		std::size_t size() const
		{
			if (not prop.is_none())
				return prop.size();
			return orig.size();
		}

		ReflectedItemBase<cv_property> get_item(const std::size_t key)
		{
			if (key >= this->orig.size())//avoid exception on too far access by index
				return {cv_property::none_property(), this->prop[key]};
			return {this->orig[key], this->prop[key]};
		}
		ReflectedItemBase<const cv_property> get_item(const std::size_t key) const
		{
			return const_ref().get_item(key);
		}
		void remove_item(const std::size_t key)
		{
			if (this->prop.is_array())
				return this->prop.remove(key);
			if (this->prop.is_none())
				if (this->orig.is_array())
				{
					this->prop = this->orig;
					return this->prop.remove(key);
				}
			throw UnexpectedItemTypeException();
		}
		std::vector<utils::keystring> get_members() const
		{
			auto members = [](const cv_property& prop) {
				if (not prop.is_dictionary())
					throw UnexpectedItemTypeException();
				std::vector<utils::keystring> out;
				out.reserve(prop.size());
				for (auto iter = prop.begin(); iter != prop.end(); ++iter)
					out.emplace_back(iter.get_key());
				return out;
			};
			if (not prop.is_none())
				return members(prop);
			return members(orig);
		}
		ReflectedItemBase<cv_property> get_item(const utils::keystring& key)
		{
			ReflectedItemBase<cv_property> item(this->orig[key], this->prop[key]);
			return item;
		}
		ReflectedItemBase<const cv_property> get_item(const utils::keystring& key) const
		{
			return const_ref().get_item(key);
		}
		void remove_item(const utils::keystring& key)
		{
			if (this->prop.is_dictionary())
				return this->prop.remove(key);
			if (this->prop.is_none())
				if (this->orig.is_dictionary())
				{
					this->prop = this->orig;
					return this->prop.remove(key);
				}
			throw UnexpectedItemTypeException();
		}

		template <typename Key>
		ReflectedItemBase<cv_property> get_bool_item(const Key& key)
		{
			auto item = get_item(key);
			if (item.is_none() or item.is_bool())
				return item;
			throw UnexpectedItemTypeException();
		}
		template <typename Key>
		ReflectedItemBase<const cv_property> get_bool_item(const Key& key) const
		{
			return const_ref().get_bool_item(key);
		}
		template <typename Key>
		ReflectedItemBase<cv_property> get_int_item(const Key& key)
		{
			auto item = get_item(key);
			if (item.is_none() or item.is_int())
				return item;
			throw UnexpectedItemTypeException();
		}
		template <typename Key>
		ReflectedItemBase<const cv_property> get_long_item(const Key& key) const
		{
			return const_ref().get_long_item(key);
		}
		template <typename Key>
		ReflectedItemBase<cv_property> get_long_item(const Key& key)
		{
			auto item = get_item(key);
			if (item.is_none() or item.is_long())
				return item;
			throw UnexpectedItemTypeException();
		}
		template <typename Key>
		ReflectedItemBase<const cv_property> get_float_item(const Key& key) const
		{
			return const_ref().get_float_item(key);
		}
		template <typename Key>
		ReflectedItemBase<cv_property> get_float_item(const Key& key)
		{
			auto item = get_item(key);
			if (item.is_none() or item.is_float())
				return item;
			throw UnexpectedItemTypeException();
		}
		template <typename Key>
		ReflectedItemBase<const cv_property> get_double_item(const Key& key) const
		{
			return const_ref().get_double_item(key);
		}
		template <typename Key>
		ReflectedItemBase<cv_property> get_double_item(const Key& key)
		{
			auto item = get_item(key);
			if (item.is_none() or item.is_double())
				return item;
			throw UnexpectedItemTypeException();
		}
		template <typename Key>
		ReflectedItemBase<const cv_property> get_int_item(const Key& key) const
		{
			return const_ref().get_int_item(key);
		}
		template <typename Key>
		ReflectedItemBase<cv_property> get_string_item(const Key& key)
		{
			auto item = get_item(key);
			if (item.is_none() or item.is_string())
				return item;
			throw UnexpectedItemTypeException();
		}
		template <typename Key>
		ReflectedItemBase<const cv_property> get_string_item(const Key& key) const
		{
			return const_ref().get_string_item(key);
		}
		ReflectedItemBase<cv_property> get_dictionary_item(const utils::keystring& key)
		{
			ReflectedItemBase<cv_property> item(this->orig[key], this->prop[key]);
			if (item.is_none() or item.is_dictionary())
				return item;
			throw UnexpectedItemTypeException();
		}
		ReflectedItemBase<const cv_property> get_dictionary_item(const utils::keystring& key) const
		{
			return const_ref().get_dictionary_item(key);
		}
		ReflectedItemBase<cv_property> get_array_item(const utils::keystring& key)
		{
			ReflectedItemBase<cv_property> item(this->orig[key], this->prop[key]);
			if (item.is_none() or item.is_array())
				return item;
			throw UnexpectedItemTypeException();
		}
		ReflectedItemBase<const cv_property> get_array_item(const utils::keystring& key) const
		{
			return const_ref().get_array_item(key);
		}
		template <typename ReflectedItemType>
		ReflectedItemType get_custom_item(const std::size_t index)
		{
			check_type<ReflectedItemType, ReflectedItemBase<cv_property> >();
			ReflectedItemType item(this->orig[index], this->prop[index]);
			return item;
		}
		template <typename ReflectedItemType>
		ReflectedItemType get_custom_item(const std::size_t index) const
		{
			return const_ref().template get_custom_item<ReflectedItemType>(index);
		}
		template <typename ReflectedItemType>
		ReflectedItemType get_custom_item(const utils::keystring& key)
		{
			check_type<ReflectedItemType, ReflectedItemBase<cv_property> >();
			ReflectedItemType item(this->orig[key], this->prop[key]);
			return item;
		}
		template <typename ReflectedItemType>
		ReflectedItemType get_custom_item(const utils::keystring& key) const
		{
			return const_ref().template get_custom_item<ReflectedItemType>(key);
		}

		template <typename ReflectedItemType>
		ReflectedItemType cast()
		{
			check_type<ReflectedItemType, ReflectedItemBase<cv_property> >();
			ReflectedItemType item(this->orig, this->prop);
			return item;
		}
		template <typename ReflectedItemType>
		ReflectedItemType cast() const
		{
			return const_ref().template cast<ReflectedItemType>();
		}
	protected:
		template <typename CastedType, typename BaseType>
		static void check_type()
		{
			static_assert(
				std::is_base_of<ReflectedItemBase<cv_property>, BaseType>::value
				or std::is_base_of<ReflectedItemBase<const cv_property>, BaseType>::value,
				"BaseType type should derived from the 'ReflectedItemBase'"
			);
			static_assert(
				std::is_base_of<BaseType, CastedType>::value
				or std::is_base_of<BaseType, CastedType>::value,
				"Casted type should derived from the 'BaseType'"
			);
		}
	};
	using ConstReflectedItem = ReflectedItemBase<const utils::property>;
	using ReflectedItem = ReflectedItemBase<utils::property>;
}
}

