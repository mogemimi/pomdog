//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "MouseCreatorDirectInput.hpp"
#include "DeviceContextDirectInput.hpp"
#include "MouseDirectInput.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Details {
namespace InputSystem {
namespace DirectInput {
//-----------------------------------------------------------------------
MouseCreatorDirectInput::MouseCreatorDirectInput(std::shared_ptr<DeviceContextDirectInput> const& deviceContextIn)
	: deviceContext(deviceContextIn)
{}
//-----------------------------------------------------------------------
std::shared_ptr<Mouse> MouseCreatorDirectInput::Create(SubsystemScheduler & scheduler)
{
	POMDOG_ASSERT(deviceContext);
	auto windowHandle = deviceContext->WindowHandle();
	auto directInput = deviceContext->GetDirectInput();

	auto mouse = std::make_shared<MouseDirectInput>(directInput, windowHandle, scheduler);
	return std::move(mouse);
}
//-----------------------------------------------------------------------
}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog
