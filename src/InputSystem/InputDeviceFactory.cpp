//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "InputDeviceFactory.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
//-----------------------------------------------------------------------
InputDeviceFactory::~InputDeviceFactory() = default;
//-----------------------------------------------------------------------
std::shared_ptr<Keyboard> InputDeviceFactory::CreateKeyboard(SubsystemScheduler & scheduler)
{
	if (!keyboardCreator) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error,
			"Cannot find keyboard device creator");
	}

	POMDOG_ASSERT(keyboardCreator);
	auto keyboard = keyboardCreator->Create(scheduler);

	POMDOG_ASSERT(keyboard);
	return std::move(keyboard);
}
//-----------------------------------------------------------------------
std::shared_ptr<Mouse> InputDeviceFactory::CreateMouse(SubsystemScheduler & scheduler)
{
	if (!mouseCreator) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error,
			"Cannot find mouse device creator");
	}

	POMDOG_ASSERT(mouseCreator);
	auto mouse = mouseCreator->Create(scheduler);

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
}// namespace Detail
}// namespace Pomdog
