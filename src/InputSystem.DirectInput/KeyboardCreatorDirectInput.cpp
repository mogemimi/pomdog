//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "KeyboardCreatorDirectInput.hpp"
#include "DeviceContextDirectInput.hpp"
#include "KeyboardDirectInput.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Details {
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
}// namespace Details
}// namespace Pomdog
