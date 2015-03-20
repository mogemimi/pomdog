//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "KeyboardCreatorDirectInput.hpp"
#include "DeviceContextDirectInput.hpp"
#include "KeyboardDirectInput.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace DirectInput {
//-----------------------------------------------------------------------
KeyboardCreatorDirectInput::KeyboardCreatorDirectInput(std::shared_ptr<DeviceContextDirectInput> const& deviceContextIn)
	: deviceContext(deviceContextIn)
{}
//-----------------------------------------------------------------------
std::shared_ptr<Keyboard> KeyboardCreatorDirectInput::Create(SubsystemScheduler & scheduler)
{
	POMDOG_ASSERT(deviceContext);
	auto windowHandle = deviceContext->WindowHandle();
	auto directInput = deviceContext->GetDirectInput();

	auto keyboard = std::make_shared<KeyboardDirectInput>(directInput, windowHandle, scheduler);
	return std::move(keyboard);
}
//-----------------------------------------------------------------------
}// namespace DirectInput
}// namespace InputSystem
}// namespace Detail
}// namespace Pomdog
