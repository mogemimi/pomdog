//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "InputDeviceFactory.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace Details {
namespace InputSystem {
//-----------------------------------------------------------------------
InputDeviceFactory::~InputDeviceFactory() = default;
//-----------------------------------------------------------------------
std::shared_ptr<Keyboard> InputDeviceFactory::CreateKeyboard()
{
	if (!keyboardCreator) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error,
			"Cannot find keyboard device creator");
	}

	POMDOG_ASSERT(keyboardCreator);
	auto keyboard = keyboardCreator->Create();

	POMDOG_ASSERT(keyboard);
	return std::move(keyboard);
}
//-----------------------------------------------------------------------
std::shared_ptr<Mouse> InputDeviceFactory::CreateMouse()
{
	if (!mouseCreator) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error,
			"Cannot find mouse device creator");
	}

	POMDOG_ASSERT(mouseCreator);
	auto mouse = mouseCreator->Create();

	POMDOG_ASSERT(mouse);
	return std::move(mouse);
}
//-----------------------------------------------------------------------
void InputDeviceFactory::AddCreator(std::unique_ptr<KeyboardCreator> && creatorIn)
{
	POMDOG_ASSERT(creatorIn);
	keyboardCreator = std::move(creatorIn);
}
//-----------------------------------------------------------------------
void InputDeviceFactory::AddCreator(std::unique_ptr<MouseCreator> && creatorIn)
{
	POMDOG_ASSERT(creatorIn);
	mouseCreator = std::move(creatorIn);
}
//-----------------------------------------------------------------------
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog
