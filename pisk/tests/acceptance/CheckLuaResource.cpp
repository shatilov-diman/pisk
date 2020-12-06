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


#include <pisk/bdd.h>

#include <pisk/utils/json_utils.h>
#include <pisk/infrastructure/Exception.h>
#include <pisk/tools/Application.h>
#include <pisk/tools/MainLoop.h>

#include <pisk/system/ResourceManager.h>

#include <pisk/script/Script.h>

#include "ServiceTest.h"

using namespace igloo;
namespace components = pisk::tools::components;
namespace script = pisk::script;

const components::DescriptionsList desc_list {
	components::Description {"loader", "lua", "lua", "get_lua_loader_factory"},
	components::Description {"service", "script", "script", "get_script_service_factory"},
};

template <typename TTestDataStream>
class TestResourcePack : public pisk::system::ResourcePack {
	virtual pisk::infrastructure::DataStreamPtr open(const std::string& rid) const noexcept threadsafe final override {
		if (rid != TTestDataStream::rid())
			return nullptr;
		return pisk::infrastructure::DataStreamPtr(new TTestDataStream());
	}
};

template <typename TDataFactory>
class TestDataStream : public pisk::infrastructure::DataStream {
	virtual std::size_t tell() const override { throw pisk::infrastructure::LogicErrorException(); }
	virtual std::size_t seek(const long, const Whence) override { throw pisk::infrastructure::LogicErrorException(); }
	virtual std::size_t read(const std::size_t, pisk::infrastructure::DataBuffer&) override { throw pisk::infrastructure::LogicErrorException(); }

	virtual pisk::infrastructure::DataBuffer readall() const override final {
		return TDataFactory::get();
	}
public:
	static std::string rid() { return TDataFactory::rid(); }
};

struct test_data_hello_world {
	static std::string rid() { return "script"; };
	static pisk::infrastructure::DataBuffer get() {
		static const std::string testdata = "\
function test_void()\n\
	return \n\
end\n\
function test_string()\n\
	return 'Hello, worlds!'\n\
end\n\
function test_number()\n\
	return 2\n\
end\n\
function test_multi()\n\
	return 42, 'qwe'\n\
end\n\
function test_sum(a, b)\n\
	return a + b\n\
end\n\
function test_dict()\n\
	local tbl = {}\n\
	tbl['qwe'] = 'asd'\n\
	tbl['zxc'] = 146\n\
	return tbl\n\
end\n\
function test_sum_array(arr)\n\
	local acc = 0\n\
	for key,value in pairs(arr) do\n\
		acc = acc + value\n\
	end\n\
	return acc\n\
end\n\
function test_mul_table(arr)\n\
	out = {}\n\
	for key,value in pairs(arr) do\n\
		out[key] = 2 * value\n\
	end\n\
	return out\n\
end\n\
function test_combo_arr(arr)\n\
	local function isnottable(t) return type(t) ~= 'table' end\n\
\n\
	local out = {}\n\
	for k1,v1 in pairs(arr) do repeat\n\
		if isnottable(v1) then\n\
			break\n\
		end\n\
		for k2,v2 in pairs(v1) do\n\
			if isnottable(out[k2]) then\n\
				out[k2] = {}\n\
			end\n\
			if isnottable(out[k2][k1]) then\n\
				out[k2][k1] = {}\n\
			end\n\
			out[k2][k1] = v2\n\
		end\n\
	until true end\n\
	return out\n\
end\n\
function test_combo_dict(dict)\n\
	local function isnottable(t) return type(t) ~= 'table' end\n\
\n\
	local out = {}\n\
	for k1,v1 in pairs(dict) do repeat\n\
		if isnottable(v1) then\n\
			break\n\
		end\n\
		for k2,v2 in pairs(v1) do\n\
			if isnottable(out[k2]) then\n\
				out[k2] = {}\n\
			end\n\
			if isnottable(out[k2][k1]) then\n\
				out[k2][k1] = {}\n\
			end\n\
			out[k2][k1] = v2\n\
		end\n\
	until true end\n\
	return out\n\
end\n\
		";
		return pisk::infrastructure::DataBuffer(testdata.begin(), testdata.end());
	}
};

struct test_data_syntax_error {
	static std::string rid() { return "script_syntax_error"; };
	static pisk::infrastructure::DataBuffer get() {
		static const std::string testdata = "\n\
function test()\n\
	local function isnottable(t) return type(t) ~= 'table' end\n\
";
		return pisk::infrastructure::DataBuffer(testdata.begin(), testdata.end());
	}
};
struct test_data_runtime_error {
	static std::string rid() { return "script_runtime_error"; };
	static pisk::infrastructure::DataBuffer get() {
		static const std::string testdata = "\n\
function test()\n\
	local nil_dict\n\
	nil_dict['key'] = 1\n\
end\n\
";
		return pisk::infrastructure::DataBuffer(testdata.begin(), testdata.end());
	}
};

std::string check_result( const pisk::script::Results& results, const pisk::script::Arguments& returns)
{
	std::string fail_msg;
	if (results.size() != returns.size())
		fail_msg = "Script returns unexpected count of values";
	else if (results != returns)
		fail_msg = "Script returns unexpected values";
	return fail_msg;
}


Context(check_script_service) {
	Spec(script_not_found) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			try {
				res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
				res_mgr->load<script::Resource>("not_found_script");
				return "resource found, but should not";
			} catch (const pisk::system::ResourceNotFound&) {
				return "";
			}
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_syntax_error) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			try {
				res_mgr->get_pack_manager().set_pack("2", std::make_unique<TestResourcePack<TestDataStream<test_data_syntax_error>>>());
				const auto& resource = res_mgr->load<script::Resource>("script_syntax_error");
				resource->make_instance();
				return "script loaded, but should not";
			} catch (const pisk::script::ScriptException& ex) {
				if (ex.error_family != pisk::script::ScriptException::syntax_error)
					return "unexpected error family";
				return "";
			}
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(runtime_syntax_error) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			try {
				res_mgr->get_pack_manager().set_pack("3", std::make_unique<TestResourcePack<TestDataStream<test_data_runtime_error>>>());
				const auto& resource = res_mgr->load<script::Resource>("script_runtime_error");
				const auto& script = resource->make_instance();
				script->execute("test_void", {});
				return "script executed, but should not";
			} catch (const pisk::script::ScriptException& ex) {
				if (ex.error_family != pisk::script::ScriptException::runtime_error)
					return "unexpected error family";
				return "";
			}
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_void) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			const auto& result = script->execute("test_void", {});
			return check_result(result, {});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_string) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			const auto& result = script->execute("test_string", {});
			return check_result(result, {"Hello, worlds!"});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_number_result) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			const auto& result = script->execute("test_number", {});
			return check_result(result, {2.});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_dictionary_result) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			const auto& result = script->execute("test_dict", {});
			pisk::utils::property dict;
			dict["qwe"] = "asd";
			dict["zxc"] = 146.;
			return check_result(result, {dict});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_multi_result) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			const auto& result = script->execute("test_multi", {});
			return check_result(result, {42., "qwe"});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_arguments) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			pisk::utils::property arguments;
			arguments[std::size_t(0)] = 15;
			arguments[1UL] = 27;
			const auto& result = script->execute("test_sum_array", {arguments});
			return check_result(result, {42.});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_dictionary_arguments) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			pisk::utils::property argument;
			argument["asd"] = 27;
			argument["zxc"] = 51;
			const auto& result = script->execute("test_mul_table", {argument});
			pisk::utils::property dict;
			dict["asd"] = 54.;
			dict["zxc"] = 102.;
			return check_result(result, {dict});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_array_combo) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			pisk::utils::property argument;
			argument[std::size_t(0)][std::size_t(0)] = "123";
			argument[std::size_t(0)][1UL] = "456";
			argument[1UL][std::size_t(0)] = 27;
			argument[2UL] = 51;
			const auto& result = script->execute("test_combo_arr", {argument});
			pisk::utils::property dict;
			dict[std::size_t(0)][std::size_t(0)] = "123";
			dict[1UL][std::size_t(0)] = "456";
			dict[std::size_t(0)][1UL] = 27.;
			return check_result(result, {dict});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(script_executed_with_dictionary_combo) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_hello_world>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			pisk::utils::property argument;
			argument["qwe"]["rty"] = "123";
			argument["qwe"]["fgh"] = "456";
			argument["asd"]["fgh"] = 27;
			argument["zxc"] = 51;
			const auto& result = script->execute("test_combo_dict", {argument});
			pisk::utils::property dict;
			dict["rty"]["qwe"] = "123";
			dict["fgh"]["qwe"] = "456";
			dict["fgh"]["asd"] = 27.;
			return check_result(result, {dict});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
};

struct test_data_non_registered_error {
	static std::string rid() { return "script"; };
	static pisk::infrastructure::DataBuffer get() {
		static const std::string testdata = "\n\
function test(vars)\n\
	return external(vars)\n\
end\n\
";
		return pisk::infrastructure::DataBuffer(testdata.begin(), testdata.end());
	}
};

struct test_data_external_with_namespace {
	static std::string rid() { return "script"; };
	static pisk::infrastructure::DataBuffer get() {
		static const std::string testdata = "\n\
function test(vars)\n\
	return xxx.external(vars)\n\
end\n\
";
		return pisk::infrastructure::DataBuffer(testdata.begin(), testdata.end());
	}
};

Context(external_functions) {
	Spec(call_non_registered_fn) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_non_registered_error>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			try {
				script->execute("test", {});
				return "script executed, but should not";
			} catch (const pisk::script::ScriptException& ex) {
				if (ex.error_family != pisk::script::ScriptException::runtime_error)
					return "unexpected error family";
				return "";
			}
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(call_registered_fn) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) -> std::string {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_non_registered_error>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			if (not script->register_external_function("", "external", [](const auto& args) {return script::Results {args[0].as_double() + 21.};}))
				return "unable to register external function";
			if (script->register_external_function("", "external", [](const auto&) {return script::Results {};}))
				return "it's should impossible to replace external function";
			const auto& result = script->execute("test", {21.});
			return check_result(result, {42.});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(call_unregistered_fn) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) -> std::string {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_non_registered_error>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			if (not script->register_external_function("", "external", [](const auto& args) {return script::Results {args[0]};}))
				return "unable to register external function";
			const auto& result = script->execute("test", {"qwe"});
			const auto& intermediate = check_result(result, {"qwe"});
			if (not intermediate.empty())
				return intermediate;
			if (not script->unregister_external_function("", "external"))
				return "unable to register external function";
			try {
				script->execute("test", {21.});
				return "script executed, but should not";
			} catch (const pisk::script::ScriptException& ex) {
				if (ex.error_family != pisk::script::ScriptException::runtime_error)
					return "unexpected error family";
			}
			return "";
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(call_registered_fn_with_namespace) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) -> std::string {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_external_with_namespace>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			if (not script->register_external_function("xxx", "external", [](const auto& args) {return script::Results {args[0].as_double() * 3};}))
				return "unable to register external function";
			if (script->register_external_function("xxx", "external", [](const auto&) {return script::Results {};}))
				return "it's should impossible to replace external function";
			const auto& result = script->execute("test", {21.});
			return check_result(result, {63.});
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
	Spec(call_unregistered_fn_with_namespace) {
		std::string fail_msg = RunAppForTestService<pisk::system::ResourceManager>(desc_list, [](const pisk::system::ResourceManagerPtr& res_mgr) -> std::string {
			res_mgr->get_pack_manager().set_pack("1", std::make_unique<TestResourcePack<TestDataStream<test_data_external_with_namespace>>>());
			const auto& resource = res_mgr->load<script::Resource>("script");
			const auto& script = resource->make_instance();
			if (not script->register_external_function("xxx", "external", [](const auto& args) {return script::Results {args[0]};}))
				return "unable to register external function";
			const auto& result = script->execute("test", {"qwe"});
			const auto& intermediate = check_result(result, {"qwe"});
			if (not intermediate.empty())
				return intermediate;
			if (not script->unregister_external_function("xxx", "external"))
				return "unable to register external function";
			try {
				script->execute("test", {21.});
				return "script executed, but should not";
			} catch (const pisk::script::ScriptException& ex) {
				if (ex.error_family != pisk::script::ScriptException::runtime_error)
					return "unexpected error family";
			}
			return "";
		});
		Assert::That(fail_msg, Is().EqualTo(""));
	}
};

