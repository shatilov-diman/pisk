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

#include "keystring.h"
#include "../infrastructure/Exception.h"

#include <assert.h>
#include <cstring>
#include <deque>
#include <map>

namespace pisk
{
namespace utils
{
	class PropertyCastException : public infrastructure::Exception
	{};

	class PropertyOutOfRangeException : public infrastructure::OutOfRangeException
	{};

	class PropertyIteratorTypeException : public infrastructure::Exception
	{};

	template <typename dict_iterator, typename arr_iterator, typename valuetype>
	class property_base_iterator
	{
		template <typename dict_iterator2, typename arr_iterator2, typename valuetype2>
		friend class property_base_iterator;

		enum class iter_type {
			_none,
			_dictionary,
			_array,
		};
		dict_iterator dict;
		arr_iterator arr;
		iter_type _type;

	public:
		property_base_iterator():
			_type(iter_type::_none)
		{}
		explicit property_base_iterator(const dict_iterator& dict):
			dict(dict),
			_type(iter_type::_dictionary)
		{}
		explicit property_base_iterator(const arr_iterator& arr):
			arr(arr),
			_type(iter_type::_array)
		{}
		keystring get_key() const {
			if (_type == iter_type::_dictionary)
				return dict->first;
			throw PropertyIteratorTypeException();
		}
		std::size_t get_index() const {
			if (_type == iter_type::_array)
				return arr->first;
			throw PropertyIteratorTypeException();
		}
		explicit operator valuetype& ()
		{
			if (_type == iter_type::_dictionary)
				return dict->second;
			if (_type == iter_type::_array)
				return arr->second;
			throw PropertyIteratorTypeException();
		}
		explicit operator const valuetype& () const
		{
			if (_type == iter_type::_dictionary)
				return dict->second;
			if (_type == iter_type::_array)
				return arr->second;
			throw PropertyIteratorTypeException();
		}
		valuetype* operator->()
		{
			if (_type == iter_type::_dictionary)
				return &dict->second;
			if (_type == iter_type::_array)
				return &arr->second;
			throw PropertyIteratorTypeException();
		}
		valuetype* operator->() const
		{
			if (_type == iter_type::_dictionary)
				return &dict->second;
			if (_type == iter_type::_array)
				return &arr->second;
			throw PropertyIteratorTypeException();
		}
		valuetype& operator*() const
		{
			if (_type == iter_type::_dictionary)
				return dict->second;
			if (_type == iter_type::_array)
				return arr->second;
			throw PropertyIteratorTypeException();
		}
		property_base_iterator& operator ++()
		{
			if (_type == iter_type::_dictionary)
				++dict;
			else if (_type == iter_type::_array)
				++arr;
			else
				throw PropertyIteratorTypeException();
			return *this;
		}
		property_base_iterator operator ++(int)
		{
			if (_type == iter_type::_dictionary)
				return property_base_iterator(dict++);
			if (_type == iter_type::_array)
				return property_base_iterator(arr++);
			throw PropertyIteratorTypeException();
		}
		template<typename another_propert_iterator>
		bool operator == (const another_propert_iterator& it) const
		{
			if (_type != static_cast<iter_type>(it._type))
				return false;
			if (_type == iter_type::_none)
				return true;
			if (_type == iter_type::_dictionary)
				return dict == it.dict;
			if (_type == iter_type::_array)
				return arr == it.arr;
			throw PropertyIteratorTypeException();
		}
		template<typename another_propert_iterator>
		bool operator != (const another_propert_iterator& it) const
		{
			return !(*this == it);
		}
	};

	class property
	{
	public:
		enum class type
		{
			_none,
			_bool,
			_int,
			_long,
			_float,
			_double,
			_string,
			_dictionary,
			_array,
		};
		using dictionary = std::map<keystring, property>;
		using array = std::map<std::size_t, property>;

		using property_iterator = property_base_iterator<dictionary::iterator, array::iterator, property>;
		using property_const_iterator = property_base_iterator<dictionary::const_iterator, array::const_iterator, const property>;

	private:
		type _type;
		union t_union
		{
			bool _bool;
			int _int;
			long _long;
			float _float;
			double _double;
			keystring* _string;
			dictionary* _dictionary;
			array* _array;
		}_union;

		void check_type(const type newtype) const
		{
			if (_type != newtype)
				throw PropertyCastException();
		}
		void check_type_iteratable() const
		{
			if (_type != type::_none && _type != type::_dictionary && _type != type::_array)
				throw PropertyCastException();
		}
		void set_type(const type newtype)
		{
			if (_type == newtype)
			{
				switch (_type)
				{
					case type::_string:     _union._string->~keystring(); break;
					case type::_dictionary: _union._dictionary->clear();  break;
					case type::_array:      _union._array->clear();       break;
					default: break;
				};
			}
			else
			{
				switch (_type)
				{
					case type::_string:     delete _union._string;     break;
					case type::_dictionary: delete _union._dictionary; break;
					case type::_array:      delete _union._array;      break;
					default: break;
				};
				_type = newtype;
				switch (_type)
				{
					case type::_none:       _union._string = nullptr;              break;
					case type::_string:     _union._string = new keystring();      break;
					case type::_dictionary: _union._dictionary = new dictionary(); break;
					case type::_array:      _union._array = new array();           break;
					default: break;
				};
			}
		}

	public:
		static const property& none_property() {
			static const property prop;
			return prop;
		}

		type get_type() const {
			return _type;
		}
		~property() {
			set_type(type::_none);
		}
		property() :
			_type(type::_none)
		{
		}
		property(const std::nullptr_t&) :
			_type(type::_none)
		{
		}
		property(const bool value) :
			_type(type::_bool)
		{
			_union._bool = value;
		}
		property(const int value) :
			_type(type::_int)
		{
			_union._int = value;
		}
		property(const long value) :
			_type(type::_long)
		{
			_union._long = value;
		}
		property(const float value) :
			_type(type::_float)
		{
			_union._float = value;
		}
		property(const double value) :
			_type(type::_double)
		{
			_union._double = value;
		}
		property(const char* value) :
			_type(type::_string)
		{
			_union._string = new keystring(value);
		}
		property(const std::string& value) :
			_type(type::_string)
		{
			_union._string = new keystring(value);
		}
		property(std::string&& value) :
			_type(type::_string)
		{
			_union._string = new keystring(std::move(value));
		}
		property(const keystring& value) :
			_type(type::_string)
		{
			_union._string = new keystring(value);
		}
		property(keystring&& value) :
			_type(type::_string)
		{
			_union._string = new keystring(std::move(value));
		}
		property(const dictionary& values) :
			_type(type::_none)
		{
			set_type(type::_dictionary);
			assert(_union._dictionary != nullptr);
			*_union._dictionary = values;
		}
		property(dictionary&& values) :
			_type(type::_none)
		{
			set_type(type::_dictionary);
			assert(_union._dictionary != nullptr);
			_union._dictionary->swap(values);
		}
		property(const array& values) :
			_type(type::_none)
		{
			set_type(type::_array);
			assert(_union._array != nullptr);
			*_union._array = values;
		}
		property(array&& values) :
			_type(type::_none)
		{
			set_type(type::_array);
			assert(_union._array != nullptr);
			_union._array->swap(values);
		}
		property(const property& prop) :
			_type(type::_none)
		{
			*this = prop;
		}
		property(property&& prop) :
			_type(type::_none)
		{
			std::swap(_type, prop._type);
			std::swap(_union, prop._union);
		}

		property_iterator begin() {
			check_type_iteratable();
			if (is_dictionary())
				return property_iterator(_union._dictionary->begin());
			if (is_array())
				return property_iterator(_union._array->begin());
			return property_iterator();
		}
		property_const_iterator begin() const {
			check_type_iteratable();
			if (is_dictionary())
				return property_const_iterator(_union._dictionary->cbegin());
			if (is_array())
				return property_const_iterator(_union._array->cbegin());
			return property_const_iterator();
		}
		property_iterator end() {
			check_type_iteratable();
			if (is_dictionary())
				return property_iterator(_union._dictionary->end());
			if (is_array())
				return property_iterator(_union._array->end());
			return property_iterator();
		}
		property_const_iterator end() const {
			check_type_iteratable();
			if (is_dictionary())
				return property_const_iterator(_union._dictionary->cend());
			if (is_array())
				return property_const_iterator(_union._array->cend());
			return property_const_iterator();
		}
		std::size_t size() const {
			if (is_none())
				return 0;
			if (is_string())
				return _union._string->size();
			if (is_dictionary())
				return _union._dictionary->size();
			if (is_array())
				return _union._array->size();
			throw PropertyCastException();
		}
		void clear() {
			set_type(type::_none);
		}
		explicit operator bool() const {
			return as_bool();
		}
		explicit operator int() const {
			return as_int();
		}
		explicit operator long() const {
			return as_long();
		}
		explicit operator float() const {
			return as_float();
		}
		explicit operator double() const {
			return as_double();
		}
		explicit operator std::string() const {
			return as_string();
		}
		explicit operator keystring() const {
			return as_keystring();
		}
		explicit operator dictionary() const {
			return as_dictionary();
		}
		explicit operator array() const {
			return as_array();
		}
		property& operator [](const char* key) { 
			return (*this)[keystring(key)];
		}
		const property& operator [](const char* key) const {
			return (*this)[keystring(key)];
		}
		property& operator [](const std::string& key) { 
			return (*this)[keystring(key)];
		}
		const property& operator [](const std::string& key) const {
			return (*this)[keystring(key)];
		}
		property& operator [](const keystring& key) { 
			if (is_none())
				set_type(type::_dictionary);
			else
				check_type(type::_dictionary);
			assert(_union._dictionary != nullptr);
			return (*_union._dictionary)[key];
		}
		const property& operator [](const keystring& key) const
		{
			if (is_none())
				return none_property();
			check_type(type::_dictionary);
			assert(_union._dictionary != nullptr);
			const auto& it = _union._dictionary->find(key);
			if (it == _union._dictionary->end())
				return none_property();
			return it->second;
		}
		property& operator [](const std::size_t key)
		{
			if (is_none())
				set_type(type::_array);
			else
				check_type(type::_array);
			assert(_union._array != nullptr);
			if (key > _union._array->size() + 0xffffff)//do not increase too much!!!
				throw infrastructure::InvalidArgumentException();
			while (key > _union._array->size())
				(*_union._array)[_union._array->size()];
			return (*_union._array)[key];
		}
		const property& operator [](const std::size_t key) const
		{
			if (is_none())
				return none_property();
			check_type(type::_array);
			assert(_union._array != nullptr);
			if (key > _union._array->size() + 0xffffff)//do not access too far!!!
				throw infrastructure::InvalidArgumentException();
			const auto& it = _union._array->find(key);
			if (it == _union._array->end())
				return none_property();
			return it->second;
		}

		property& operator=(const std::nullptr_t&)
		{
			set_type(type::_none);
			return *this;
		}
		property& operator=(const bool value) {
			set_type(type::_bool);
			_union._bool = value;
			return *this;
		}
		property& operator=(const int value) {
			set_type(type::_int);
			_union._int = value;
			return *this;
		}
		property& operator=(const long value) {
			set_type(type::_long);
			_union._long = value;
			return *this;
		}
		property& operator=(const float value) {
			set_type(type::_float);
			_union._float = value;
			return *this;
		}
		property& operator=(const double value) {
			set_type(type::_double);
			_union._double = value;
			return *this;
		}
		property& operator=(const char* value) {
			set_type(type::_string);
			new (_union._string) keystring(value);
			return *this;
		}
		property& operator=(const std::string& value) {
			set_type(type::_string);
			new (_union._string) keystring(value);
			return *this;
		}
		property& operator=(const keystring& value) {
			set_type(type::_string);
			new (_union._string) keystring(value);
			return *this;
		}
		property& operator=(const dictionary& properties) {
			set_type(type::_dictionary);
			assert(_union._dictionary != nullptr);
			*_union._dictionary = properties;
			return *this;
		}
		property& operator=(const array& properties) {
			set_type(type::_array);
			assert(_union._array != nullptr);
			*_union._array = properties;
			return *this;
		}
		property& operator=(const property& property) {
			if (this == &property)
				return *this;
			if (property._type == type::_string)
				*this = property.as_keystring_cref();
			else if (property._type == type::_dictionary)
				*this = property.as_dictionary_cref();
			else if (property._type == type::_array)
				*this = property.as_array_cref();
			else {
				set_type(property._type);
				::memcpy(&_union, &property._union, sizeof(t_union));
			}
			return *this;
		}
		property& operator=(property&& property) {
			if (this == &property)
				return *this;
			std::swap(_type, property._type);
			std::swap(_union, property._union);
			return *this;
		}
		bool operator ==(const property& property) const {
			if (_type != property._type)
				return false;
			switch(_type) {
				case type::_none: return true;
				case type::_bool: return _union._bool == property._union._bool;
				case type::_int: return _union._int == property._union._int;
				case type::_long: return _union._long == property._union._long;
				case type::_float: return _union._float == property._union._float;
				case type::_double: return _union._double == property._union._double;
				case type::_string: return *_union._string == *property._union._string;
				case type::_dictionary: return *_union._dictionary == *property._union._dictionary;
				case type::_array: return *_union._array == *property._union._array;

				default: assert(!"Unsupported property_tree type"); return false;
			}
		}
		bool operator !=(const property& property) const {
			return not (*this == property);
		}

		bool is_none() const {
			return get_type() == type::_none;
		}
		bool is_bool() const {
			return get_type() == type::_bool;
		}
		bool is_int() const {
			return get_type() == type::_int;
		}
		bool is_long() const {
			return get_type() == type::_long;
		}
		bool is_float() const {
			return get_type() == type::_float;
		}
		bool is_double() const {
			return get_type() == type::_double;
		}
		bool is_string() const {
			return get_type() == type::_string;
		}
		bool is_dictionary() const {
			return get_type() == type::_dictionary;
		}
		bool is_array() const {
			return get_type() == type::_array;
		}

		bool as_bool() const {
			check_type(type::_bool);
			return _union._bool;
		}
		int as_int() const {
			check_type(type::_int);
			return _union._int;
		}
		long as_long() const {
			check_type(type::_long);
			return _union._long;
		}
		float as_float() const {
			check_type(type::_float);
			return _union._float;
		}
		double as_double() const {
			check_type(type::_double);
			return _union._double;
		}
		const std::string as_string() const {
			return  as_keystring_cref().get_content();
		}
		const keystring as_keystring() const {
			return as_keystring_cref();
		}
		const dictionary as_dictionary() const {
			return as_dictionary_cref();
		}
		const array as_array() const {
			return as_array_cref();
		}

	private:
		const keystring& as_keystring_cref() const {
			check_type(type::_string);
			assert(_union._string != nullptr);
			return *_union._string;
		}
		const dictionary& as_dictionary_cref() const {
			check_type(type::_dictionary);
			assert(_union._dictionary != nullptr);
			return *_union._dictionary;
		}
		const array& as_array_cref() const {
			check_type(type::_array);
			assert(_union._array != nullptr);
			return *_union._array;
		}

	public:
		bool contains(const char* key) const {
			return contains(keystring(key));
		}
		bool contains(const std::string& key) const {
			return contains(keystring(key));
		}
		bool contains(const keystring& key) const {
			if (is_none())
				return false;
			check_type(type::_dictionary);
			return _union._dictionary->find(key) != _union._dictionary->end();
		}

		void remove(const std::size_t key) const {
			UNUSED(key);
			check_type(type::_array);
			throw infrastructure::Exception();//Not implemented
		}
		void remove(const keystring& key) const {
			check_type(type::_dictionary);
			auto it = _union._dictionary->find(key);
			if (it == _union._dictionary->end())
				throw PropertyOutOfRangeException();
			_union._dictionary->erase(it);
		}

		static property merge(const property& original, const property& admixture)
		{
			if (original.is_none())
				return admixture;
			if (admixture.is_none())
				return original;
			if (original.get_type() != admixture.get_type())
				throw PropertyCastException();
			if (original.is_dictionary())
			{
				property out = original;
				for (auto it = admixture.begin(); it != admixture.end(); ++it)
					out[it.get_key()] = merge(original[it.get_key()], *it);
				return out;
			}
			if (original.is_array())
			{
				property out = original;
				for (auto it = admixture.begin(); it != admixture.end(); ++it)
					out[it.get_index()] = merge(out[it.get_index()], *it);
				return out;
			}
			return admixture;
		}
	};
}
}
