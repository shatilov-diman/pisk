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
#include <pisk/system/ResourceManager.h>

#include <functional>
#include <sstream>
#include <fstream>
#include <string>

using namespace igloo;

static const std::string packid = "pack1";
static const std::string packid2 = "pack2";
static const std::string packid2_override = "pack2_override";

struct test_data {
	static std::string rid() { return "resid"; };
	static std::string res() { return "binary"; };
	static pisk::infrastructure::DataBuffer get() {
		static const std::string testdata = res();
		return pisk::infrastructure::DataBuffer(testdata.begin(), testdata.end());
	}
};

struct test_data2 {
	static std::string rid() { return "resid2"; };
	static std::string res() { return "data"; };
	static pisk::infrastructure::DataBuffer get() {
		static const std::string testdata = res();
		return pisk::infrastructure::DataBuffer(testdata.begin(), testdata.end());
	}
};
struct test_data2_override {
	static std::string rid() { return "resid2"; };
	static std::string res() { return "data_override"; };
	static pisk::infrastructure::DataBuffer get() {
		static const std::string testdata = res();
		return pisk::infrastructure::DataBuffer(testdata.begin(), testdata.end());
	}
};

class TestCustomDataParser
{
public:
	static pisk::infrastructure::DataBuffer parse(const pisk::infrastructure::DataStream& data) {
		return data.readall();
	}
};
class UnknownResource :
	public pisk::system::Resource
{
public:
	constexpr static const char* resource_type = "unknown";
};

class BinaryResource :
	public pisk::system::Resource
{
	pisk::infrastructure::DataStreamPtr data;
public:
	constexpr static const char* resource_type = "binary";

	explicit BinaryResource(pisk::infrastructure::DataStreamPtr&& stream):
		data(std::move(stream))
	{}

	pisk::infrastructure::DataBuffer readall() {
		return data->readall();
	}
};

class SuperBinaryResource :
	public BinaryResource
{
};

class BinaryResourceLoader :
	public pisk::system::ResourceLoader
{
	pisk::infrastructure::DataStreamPtr data;

	virtual void release() override {
		delete this;
	}

	virtual bool can_load(const pisk::infrastructure::DataStreamPtr&) const noexcept threadsafe final override {
		return true;
	}
	virtual pisk::system::ResourcePtr load(pisk::infrastructure::DataStreamPtr&& stream) const threadsafe final override {
		return std::make_shared<BinaryResource>(std::move(stream));
	}
public:
	constexpr static const char* resource_type = "binary";
};
class SuperBinaryResourceLoader :
	public pisk::system::ResourceLoader
{
	virtual void release() override {
		delete this;
	}

	virtual bool can_load(const pisk::infrastructure::DataStreamPtr&) const noexcept threadsafe final override {
		return false;
	}
	virtual pisk::system::ResourcePtr load(pisk::infrastructure::DataStreamPtr&& stream) const threadsafe final override {
		return std::make_shared<BinaryResource>(std::move(stream));
	}
public:
	constexpr static const char* resource_type = BinaryResourceLoader::resource_type;
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

class TestFileDataStream : public pisk::infrastructure::DataStream {
	typedef pisk::infrastructure::DataBuffer::value_type value_type;
	typedef std::istreambuf_iterator<std::ifstream::char_type> iterator;

	std::ifstream file;

public:
	static pisk::infrastructure::DataStreamPtr open(const std::string& rid)
	{
		std::ifstream file(rid.c_str(), std::ios::binary);
		if (file.is_open() == false)
			return nullptr;
		return std::unique_ptr<TestFileDataStream>(new TestFileDataStream(std::move(file)));
	}

private:

	TestFileDataStream(std::ifstream&& file) :
		file(std::move(file))
	{}

	const std::streamsize size() const {
		std::ifstream& ref = const_cast<std::ifstream&>(file);
		const std::streampos pos = ref.tellg();
		ref.seekg(0, std::ios::end);
		const std::streampos content_end = ref.tellg();
		ref.seekg(pos, std::ios::beg);
		return content_end;
	}

	virtual std::size_t tell() const override { throw pisk::infrastructure::LogicErrorException(); }
	virtual std::size_t seek(const long, const Whence) override { throw pisk::infrastructure::LogicErrorException(); }
	virtual std::size_t read(const std::size_t, pisk::infrastructure::DataBuffer&) override { throw pisk::infrastructure::LogicErrorException(); }

	virtual pisk::infrastructure::DataBuffer readall() const override final {
		std::ifstream& ref = const_cast<std::ifstream&>(file);
		const std::streampos pos = ref.tellg();

		pisk::infrastructure::DataBuffer out;
		out.reserve(static_cast<std::size_t>(size()));
		std::copy(iterator(ref.rdbuf()), iterator(), std::back_inserter(out));

		ref.seekg(pos);
		return out;
	}
};



template <typename TTestDataStream>
class TestResourcePack : public pisk::system::ResourcePack {
	virtual pisk::infrastructure::DataStreamPtr open(const std::string& rid) const noexcept threadsafe final override {
		if (rid != TTestDataStream::rid())
			return nullptr;
		return pisk::infrastructure::DataStreamPtr(new TTestDataStream());
	}
};

template <typename TTestDataStream>
class TestFileResourcePack : public pisk::system::ResourcePack {
	virtual pisk::infrastructure::DataStreamPtr open(const std::string& rid) const noexcept threadsafe final override {
		return TTestDataStream::open(rid);
	}
};

class TestStreamDecoder:
	public pisk::infrastructure::DataStream
{
	pisk::infrastructure::DataStreamPtr raw;
public:
	explicit TestStreamDecoder(pisk::infrastructure::DataStreamPtr&& data):
		raw(std::move(data))
	{}
	virtual std::size_t tell() const override { throw pisk::infrastructure::LogicErrorException(); }
	virtual std::size_t seek(const long, const Whence) override { throw pisk::infrastructure::LogicErrorException(); }
	virtual std::size_t read(const std::size_t, pisk::infrastructure::DataBuffer&) override { throw pisk::infrastructure::LogicErrorException(); }

	virtual pisk::infrastructure::DataBuffer readall() const final override
	{
		return raw->readall();
	}
};

std::shared_ptr<pisk::system::ResourceManager> CreateResourceManager();

Describe(ResourceManager)
{
	std::shared_ptr<pisk::system::ResourceManager> res_manager_instance = CreateResourceManager();

	pisk::system::ResourceManager& res_manager() {
		return *res_manager_instance;
	}
	pisk::system::ResourcePackManager& res_packs() {
		return res_manager().get_pack_manager();
	}
	pisk::system::ResourceLoaderRegistry& res_loaders() {
		return res_manager().get_loader_registry();
	}
	void SetUp() {
		Root().res_packs().clear();
	}
	Context(no_pack_no_loaders) {
		When(load_resource) {
			Then(throw_ResourceNotFound_exception) {
				AssertThrows(pisk::system::ResourceNotFound, Root().res_manager().load<BinaryResource>(""));
			}
		};
		When(add_loader) {
			void SetUp() {
				Root().res_loaders().register_resource_loader(BinaryResource::resource_type, {nullptr, std::make_shared<BinaryResourceLoader>()});
			}
			Then(resources_still_not_found) {
				AssertThrows(pisk::system::ResourceNotFound, Root().res_manager().load<BinaryResource>(""));
			}
		};
		When(add_pack_and_register_notsupport_format_loader) {
			void SetUp() {
				Root().res_packs().set_pack(packid, std::make_unique<TestResourcePack<TestDataStream<test_data>>>());
				Root().res_loaders().register_resource_loader(BinaryResource::resource_type, {nullptr, std::make_shared<SuperBinaryResourceLoader>()});
			}
			Then(resource_not_supported) {
				AssertThrows(pisk::system::ResourceFormatNotSupported, Root().res_manager().load<BinaryResource>(test_data::rid()));
			}
			When(register_needed_loader_with_same_type) {
				void SetUp() {
					Root().res_loaders().register_resource_loader(BinaryResource::resource_type, {nullptr, std::make_shared<BinaryResourceLoader>()});
				}
				Then(resource_loaded) {
					Assert::That(Root().res_manager().load<BinaryResource>(test_data::rid())->readall(), EqualsContainer(test_data::res()));
				}
			};
		};
	};
	Context(no_pack) {
		void SetUp() {
			Root().res_loaders().register_resource_loader(BinaryResource::resource_type, {nullptr, std::make_shared<BinaryResourceLoader>()});
		}
		When(load_resource) {
			Then(throw_ResourceNotFound_exception) {
				AssertThrows(pisk::system::ResourceNotFound, Root().res_manager().load<BinaryResource>(""));
			}
		};
		When(remove_pack) {
			Then(no_exception) {
				Root().res_packs().remove_pack("");
			}
		};
		When(add_pack) {
			void SetUp() {
				Root().res_packs().set_pack("", std::make_unique<TestResourcePack<TestDataStream<test_data>>>());
			}
			Then(it_can_be_used_to_load) {
				Assert::That(Root().res_manager().load<BinaryResource>(test_data::rid())->readall(), EqualsContainer(test_data::res()));
			}
		};
	};
	Context(one_pack_without_loaders) {
		void SetUp() {
			Root().res_packs().set_pack(packid, std::make_unique<TestResourcePack<TestDataStream<test_data>>>());
		}
		Spec(try_to_load) {
			AssertThrows(pisk::system::ResourceFormatNotSupported, Root().res_manager().load<BinaryResource>(test_data::rid()));
		}
		When(add_loader) {
			void SetUp() {
				Root().res_loaders().register_resource_loader(BinaryResource::resource_type, {nullptr, std::make_shared<BinaryResourceLoader>()});
			}
			Then(resource_can_be_loaded) {
				Root().res_manager().load<BinaryResource>(test_data::rid());
			}
		};
	};
	Context(one_pack_one_loader) {
		void SetUp() {
			Root().res_loaders().register_resource_loader(BinaryResource::resource_type, {nullptr, std::make_shared<BinaryResourceLoader>()});
			Root().res_packs().set_pack(packid, std::make_unique<TestResourcePack<TestDataStream<test_data>>>());
		}
		Spec(existent_can_be_read) {
			Assert::That(Root().res_manager().load<BinaryResource>(test_data::rid())->readall(), EqualsContainer(test_data::res()));
		}
		Spec(try_load_non_existent) {
			AssertThrows(pisk::system::ResourceNotFound, Root().res_manager().load<BinaryResource>("non_exists_resource_id"));
		}
		Spec(unable_load_unknown_resource_type) {
			AssertThrows(pisk::system::ResourceFormatNotSupported, Root().res_manager().load<UnknownResource>(test_data::rid()));
		}
		Spec(unable_to_cast_to_wrong_resource) {
			AssertThrows(pisk::infrastructure::InvalidArgumentException, Root().res_manager().load<SuperBinaryResource>(test_data::rid()));
		}
		When(remove_pack) {
			void SetUp() {
				Root().res_packs().remove_pack(packid);
			}
			Then(resource_not_found) {
				AssertThrows(pisk::system::ResourceNotFound, Root().res_manager().load<BinaryResource>(test_data::rid()));
			}
		};
	};
	Context(two_pack_one_loader) {
		void SetUp() {
			Root().res_loaders().register_resource_loader(BinaryResource::resource_type, {nullptr, std::make_shared<BinaryResourceLoader>()});
			Root().res_packs().set_pack(packid, std::make_unique<TestResourcePack<TestDataStream<test_data>>>());
			Root().res_packs().set_pack(packid2, std::make_unique<TestResourcePack<TestDataStream<test_data2>>>());
		}
		Spec(resources_can_be_readed) {
			Assert::That(Root().res_manager().load<BinaryResource>(test_data::rid())->readall(), EqualsContainer(test_data::res()));
			Assert::That(Root().res_manager().load<BinaryResource>(test_data2::rid())->readall(), EqualsContainer(test_data2::res()));
		}
		When(remove_first_pack) {
			void SetUp() {
				Root().res_packs().remove_pack(packid);
			}
			Then(first_cannot_loaded_but_second_can) {
				AssertThrows(pisk::system::ResourceNotFound, Root().res_manager().load<BinaryResource>(test_data::rid()));
				Assert::That(Root().res_manager().load<BinaryResource>(test_data2::rid())->readall(), EqualsContainer(test_data2::res()));
			}
		};
		When(remove_second_pack) {
			void SetUp() {
				Root().res_packs().remove_pack(packid2);
			}
			Then(first_can_loaded_but_second_cannot) {
				Assert::That(Root().res_manager().load<BinaryResource>(test_data::rid())->readall(), EqualsContainer(test_data::res()));
				AssertThrows(pisk::system::ResourceNotFound, Root().res_manager().load<BinaryResource>(test_data2::rid()));
			}
		};
	};
	Context(two_packs_with_same_resid) {
		void SetUp() {
			Root().res_loaders().register_resource_loader(BinaryResource::resource_type, {nullptr, std::make_shared<BinaryResourceLoader>()});
			Root().res_packs().set_pack(packid2, std::make_unique<TestResourcePack<TestDataStream<test_data2>>>());
			Root().res_packs().set_pack(packid2_override, std::make_unique<TestResourcePack<TestDataStream<test_data2_override>>>());
		}
		When(get_resource) {
			Then(test_data2_override___should_be_received) {
				Assert::That(Root().res_manager().load<BinaryResource>(test_data2::rid())->readall(), EqualsContainer(test_data2_override::res()));
			}
		};
		When(remove_first_pack) {
			void SetUp() {
				Root().res_packs().remove_pack(packid2_override);
			}
			Then(test_data2___should_be_received) {
				Assert::That(Root().res_manager().load<BinaryResource>(test_data2::rid())->readall(), EqualsContainer(test_data2::res()));
			}
		};
	};
};

