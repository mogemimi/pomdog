// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "MouseCreatorDirectInput.hpp"
#include "DeviceContextDirectInput.hpp"
#include "MouseDirectInput.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
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
}// namespace Detail
}// namespace Pomdog
