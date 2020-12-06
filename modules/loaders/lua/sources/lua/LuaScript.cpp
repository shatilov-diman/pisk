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

#include <pisk/utils/algorithm_utils.h>
#include <pisk/utils/property_tree.h>
#include <pisk/infrastructure/Logger.h>

#include <pisk/script/Script.h>

#include <map>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#ifndef LUA_OK
#	define LUA_OK 0
#endif

namespace pisk
{
namespace script
{
namespace loaders
{
	class LuaStackChecker
	{
		lua_State* state;
		int top;
	public:
		explicit LuaStackChecker(lua_State* state):
			state(state),
			top(lua_gettop(state))
		{
			if (state == nullptr)
				throw infrastructure::NullPointerException();
		}

		~LuaStackChecker()
		{
			if (lua_gettop(state) != top)
			{
				logger::critical("lua", "Lua stack-checker just detected stack leak. Restore stack");
				lua_settop(state, top);
			}
		}
	};

	class LuaScript final :
		public script::Script
	{
		std::map<std::string, ExternalFunction> functions;
		lua_State* state;

	public:
		explicit LuaScript(const infrastructure::DataBuffer& data) :
			state(luaL_newstate())
		{
			if (state == nullptr)
				throw infrastructure::InitializeError();

			LuaStackChecker checker(state);
			try
			{
				logger::debug("lua", "ctor: init lua script");

				load_content(data);
				load_standart_libs();
				run_main();
			}
			catch (const infrastructure::InitializeError&)
			{
				logger::error("lua", "ctor: exception occured while init lua script");
				lua_close(state);
				throw;
			}
		}

		virtual ~LuaScript()
		{
			logger::debug("lua", "dtor: deinit lua script");
			lua_close(state);
		}

	private:
		void load_content(const infrastructure::DataBuffer& data)
		{
			std::string content {data.begin(), data.end()};
			const auto load_result = luaL_loadstring(state, content.c_str());
			if (load_result != LUA_OK)
			{
				const utils::keystring& error_msg = pop_result().as_keystring();
				logger::debug("lua", "luaL_loadstring ended with error: {}, {}", load_result, error_msg);
				throw ScriptException (error_msg, to_error(load_result));
			}
		}
		void load_standart_libs()
		{
			luaL_openlibs(state);
		}
		void run_main()
		{
			const auto run_main_result = lua_pcall(state, 0, 0, 0);
			if (run_main_result != LUA_OK)
			{
				const utils::keystring& error_msg = pop_result().as_keystring();
				logger::debug("lua", "lua_pcall entry_point ended with error: {}, {}", run_main_result, error_msg);
				throw ScriptException (error_msg, to_error(run_main_result));
			}
		}

	private:
		virtual bool register_external_function(const utils::keystring& nameprefix, const utils::keystring& name, ExternalFunction function) final override
		{
			logger::debug("lua", "Register external function '{}.{}'", nameprefix, name);
			const std::string fullname = nameprefix.get_content() + '.' + name.get_content();
			if (functions[fullname] != nullptr)
			{
				logger::debug("lua", "External function '{}.{}' already exists", nameprefix, name);
				return false;
			}
			functions[fullname] = function;

			LuaStackChecker checker(state);

			if (nameprefix.empty())
			{
				lua_pushlightuserdata(state, static_cast<void*>(this));
				lua_pushstring(state, fullname.c_str());
				lua_pushcclosure(state, &LuaScript::closure, 2);
				lua_setglobal(state, name.c_str());//pops one element from the stack
			}
			else
			{
				lua_getglobal(state, nameprefix.c_str());//try to get table on the top
				if (lua_isnil(state, -1))
				{
					lua_pop(state, 1);//nill on the top; pop it

					lua_newtable(state);
					lua_pushvalue(state, -1);//duplicate for lua_setglobal
					lua_setglobal(state, nameprefix.c_str());//pops one element from the stack
				}
				lua_pushstring(state, name.c_str());//table key

				lua_pushlightuserdata(state, static_cast<void*>(this));
				lua_pushstring(state, fullname.c_str());
				lua_pushcclosure(state, &LuaScript::closure, 2);//table value

				lua_settable(state, -3);
				lua_pop(state, 1);//table on the top; pop it
			}
			return true;
		}
		virtual bool unregister_external_function(const utils::keystring& nameprefix, const utils::keystring& name) final override
		{
			logger::debug("lua", "Unregister external function '{}.{}'", nameprefix, name);
			const std::string fullname = nameprefix.get_content() + '.' + name.get_content();
			if (functions[fullname] == nullptr)
			{
				logger::debug("lua", "External function '{}.{}' not found", nameprefix, name);
				return false;
			}
			functions.erase(fullname);

			LuaStackChecker checker(state);

			if (nameprefix.empty())
			{
				lua_getglobal(state, name.c_str());
				if (lua_isnil(state, -1))
					logger::warning("lua", "unregister: unable to locate '{}.{}'", nameprefix, name);
				else
				{
					lua_pop(state, 1);
					lua_pushnil(state);
				}
				lua_setglobal(state, name.c_str());
			}
			else
			{
				lua_getglobal(state, nameprefix.c_str());//try to get table on the top
				if (lua_isnil(state, -1))
					logger::warning("lua", "unregister: unable to locate '{}.{}'", nameprefix, name);
				else
				{
					lua_pushstring(state, name.c_str());//table key
					lua_pushnil(state);//table value
					lua_settable(state, -3);
				}
				lua_pop(state, 1);//table or nil on the top; pop it
			}
			return true;
		}

		static int closure(lua_State* state)
		{
			logger::spam("lua", "closure");

			const int this_idx = lua_upvalueindex(1);
			const int fn_name_idx = lua_upvalueindex(2);

			if (not lua_isuserdata(state, this_idx) or not lua_isstring(state, fn_name_idx))
			{
				logger::error("lua", "Unepxected closure variables");
				return luaL_error(state, "Unepxected closure variables");
			}

			LuaScript* pthis = reinterpret_cast<LuaScript*>(lua_touserdata(state, this_idx));
			const char* function_name = lua_tostring(state, fn_name_idx);
			if (function_name == nullptr)
			{
				logger::error("lua", "closure contains nullptr function name");
				return luaL_error(state, "closure contains nullptr function name");
			}

			logger::spam("lua", "closure '{}'", function_name);
			int result = pthis->closure(function_name);
			logger::spam("lua", "closure '{}' complete with return {} args", function_name, result);
			return result;
		}
		int closure(const std::string& function_name)
		{
			logger::spam("lua", "External function '{}' call", function_name);
			const int top = lua_gettop(state);
			try
			{
				if (functions[function_name] == nullptr)
					throw infrastructure::NullPointerException();
				const auto& results = functions[function_name](pop_results(top));
				push_arguments(results);
				return results.size();
			}
			catch (const infrastructure::InvalidArgumentException&)
			{
				logger::warning("lua", "Unexpected arguments received while run '{}'", function_name);
				lua_settop(state, top);
				return luaL_error(state, "Unexpected arguments received while run '{}'", function_name.c_str());
			}
			catch (const infrastructure::Exception&)
			{
				logger::warning("lua", "Unexpected exception received while run '{}'", function_name);
				lua_settop(state, top);
				return luaL_error(state, "Unexpected exception received while run '{}'", function_name.c_str());
			}
			catch (const std::exception& ex)
			{
				logger::warning("lua", "Unexpected std::exception received while run '{}': {}", function_name, ex.what());
				lua_settop(state, top);
				throw;
			}
			catch (...)
			{
				logger::warning("lua", "Unexpected arguments received while run '{}'", function_name);
				lua_settop(state, top);
				throw;
			}
			return 0;
		}

		virtual Results execute(const utils::keystring& function, const Arguments& arguments) final override
		{
			LuaStackChecker checker(state);

			const int top = lua_gettop(state);
			try
			{
				logger::spam("lua", "Execute function '{}'", function);

				push_function(function);
				push_arguments(arguments);
				execute_function(arguments.size());

				return pop_results(get_returns_count(top));
			}
			catch (...)
			{
				logger::error("lua", "exception detected while execute function '{}'", function);
				lua_settop(state, top);
				throw;
			}
		}
		void push_function(const utils::keystring& function)
		{
			lua_getglobal(state, function.c_str());
			if (lua_isnil(state, -1))
			{
				const utils::keystring error_msg { utils::string::format("Function '{}' not found", function.c_str()) };
				logger::error("lua", error_msg.get_content());
				throw ScriptException (error_msg, ScriptException::runtime_error);
			}
		}
		void execute_function(const std::size_t arguments_count)
		{
			const auto result = lua_pcall(state, arguments_count, LUA_MULTRET, 0);
			if (result != LUA_OK)
			{
				const utils::keystring& error_msg = pop_result().as_keystring();
				logger::error("lua", "lua_pcall ended with error: {}, {}", result, error_msg);
				throw ScriptException (error_msg, to_error(result));
			}
		}

		std::size_t get_returns_count(const int top)
		{
			const int result_count = lua_gettop(state) - top;
			assert(result_count >= 0);
			return static_cast<std::size_t>(result_count);
		}

		void push_arguments(const Arguments& arguments)
		{
			for (const auto& arg : arguments)
				push_argument(arg);
		}

		void push_argument(const utils::property& arg)
		{
			switch (arg.get_type())
			{
				case utils::property::type::_none:
					lua_pushnil(state);
					break;
				case utils::property::type::_bool:
					lua_pushboolean(state, static_cast<int>(arg.as_bool()));
					break;
				case utils::property::type::_int:
					lua_pushinteger(state, static_cast<int>(arg.as_int()));
					break;
				case utils::property::type::_long:
					lua_pushinteger(state, static_cast<int>(arg.as_long()));
					break;
				case utils::property::type::_float:
					lua_pushnumber(state, static_cast<double>(arg.as_float()));
					break;
				case utils::property::type::_double:
					lua_pushnumber(state, static_cast<double>(arg.as_double()));
					break;
				case utils::property::type::_string:
					lua_pushstring(state, arg.as_keystring().c_str());
					break;
				case utils::property::type::_dictionary:
					{
						lua_newtable(state);
						const int top = lua_gettop(state);
						for (auto it = arg.begin(); it != arg.end(); ++it)
						{
							lua_pushstring(state, it.get_key().c_str());
							push_argument(*it);
							lua_settable(state, top);
						}
					}
					break;
				case utils::property::type::_array:
					{
						lua_newtable(state);
						const int top = lua_gettop(state);
						for (std::size_t index = 0; index < arg.size(); ++index)
						{
							lua_pushinteger(state, static_cast<int>(1+index));
							push_argument(arg[index]);
							lua_settable(state, top);
						}
					}
					break;
				default:
					logger::error("lua", "Unexpected property type detected while push arguments");
					break;
			}
		}
		Results pop_results(const std::size_t result_count)
		{
			Results out;
			out.resize(result_count);
			for (std::size_t index = result_count; index > 0; --index)
				out[index - 1] = pop_result();
			return out;
		}
		utils::property pop_result()
		{
			utils::property out;
			const int type = lua_type(state, -1);
			switch (type)
			{
				case LUA_TNIL:
					break;
				case LUA_TBOOLEAN:
					out = lua_toboolean(state, -1) == 0 ? false : true;
					break;
				case LUA_TNUMBER:
					out = static_cast<double>(lua_tonumber(state, -1));
					break;
				case LUA_TSTRING:
					out = lua_tostring(state, -1);
					break;
				case LUA_TTABLE:
					lua_pushnil(state);
					while (lua_next(state, -2) != 0)
					{
						if (lua_isnumber(state, -2))
						{
							const auto& key = static_cast<std::ptrdiff_t>(lua_tointeger(state, -2));
							if (key > 0)
								out[key-1] = pop_result();
							else
							{
								logger::warning("lua", "Lua array keys starts from 1. Skip it");
								pop_result();
							}
						}
						else if (lua_isstring(state, -2))
						{
							const utils::keystring key {lua_tostring(state, -2)};
							out[key] = pop_result();
						}
						else
						{
							logger::warning("lua", "Unsupported lua type detected for key of table: {}", lua_type(state, -2));
							lua_pop(state, 1);
						}
					}
					break;
				default:
					logger::error("lua", "Unsupported lua type detected while pop arguments: {}", type);
					break;
			}
			lua_pop(state, 1);
			return out;
		}

		static ScriptException::ErrorFamily to_error(const int lua_error)
		{
			switch (lua_error)
			{
				case LUA_OK:        return ScriptException::success;
				case LUA_YIELD:     return ScriptException::unknown;
				case LUA_ERRRUN:    return ScriptException::runtime_error;
				case LUA_ERRSYNTAX: return ScriptException::syntax_error;
				case LUA_ERRMEM:    return ScriptException::memory_error;
				case LUA_ERRERR:    return ScriptException::internal_error;
			};
			return ScriptException::unknown;
		}
	};

	script::ScriptPtr make_script_instance(const infrastructure::DataBuffer& data)
	{
		return std::make_unique<LuaScript>(data);
	}
}
}
}

