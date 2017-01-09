// Project pisk
// Copyright (C) 2016-2017 Dmitry Shatilov
//
// This file is a part of the module audio of the project pisk.
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


#include <pisk/bdd.h>

#include <pisk/system/PatchPtr.h>

#include "../../sources/audio/EngineStrategy.h"

using namespace igloo;
using pisk::utils::property;
using pisk::system::PatchPtr;
using pisk::system::PatchRecipient;

class TestPatchRecipient :
	public PatchRecipient
{
	PatchPtr _last_patch;

	virtual void push(const PatchPtr& patch) noexcept threadsafe
	{
		_last_patch = patch;
	}
public:
	PatchPtr last_patch() const
	{
		return _last_patch;
	}
};

Describe(Engine) {
	std::unique_ptr<pisk::system::EngineStrategy> audio;
	TestPatchRecipient patch_recipient;

	void SetUp() {
		auto source_loader = [](const pisk::utils::keystring&) -> pisk::audio::AudioSourcePtr {
			return {};
		};
		audio = std::make_unique<pisk::audio::EngineStrategy>(source_loader, patch_recipient);
	}

	When(update_model_with_null) {
		Then(exception_thrown) {
			AssertThrows(
				pisk::infrastructure::NullPointerException,
				Root().audio->patch_scene({})
			);
		}
	};
	When(update_model_with_none) {
		void SetUp() {
			Root().audio->patch_scene(std::make_shared<property>());
			Root().audio->update();
		}
		Then(last_patch_is_nullptr) {
			Assert::That(Root().patch_recipient.last_patch(), Is().EqualTo(nullptr));
		}
	};
	When(update_model_with_int_property) {
		Then(LogicErrorException_thrown) {
			AssertThrows(
				pisk::infrastructure::LogicErrorException,
				Root().audio->patch_scene(std::make_shared<property>(1))
			);
			Root().audio->update();
			Assert::That(Root().patch_recipient.last_patch(), Is().EqualTo(nullptr));
		}
	};
};

