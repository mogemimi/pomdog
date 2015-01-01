//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "DeviceContextDirectInput.hpp"
#include "../Platform.Win32/GameWindowWin32.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Details {
namespace InputSystem {
namespace DirectInput {
//-----------------------------------------------------------------------
DeviceContextDirectInput::DeviceContextDirectInput(HINSTANCE hInstance, HWND windowHandleIn)
	: windowHandle(windowHandleIn)
{
	POMDOG_ASSERT(hInstance);
	POMDOG_ASSERT(windowHandle);

	auto hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		&directInput, nullptr);

	if (FAILED(hr)) {
		// error: FUS RO DAH!
		///@todo Not implemented
	}
}
//-----------------------------------------------------------------------
DeviceContextDirectInput::~DeviceContextDirectInput() = default;
//-----------------------------------------------------------------------
HWND DeviceContextDirectInput::WindowHandle() const
{
	POMDOG_ASSERT(windowHandle);
	return windowHandle;
}
//-----------------------------------------------------------------------
IDirectInput8* DeviceContextDirectInput::GetDirectInput() const
{
	POMDOG_ASSERT(directInput);
	return directInput.Get();
}
//-----------------------------------------------------------------------
}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog
