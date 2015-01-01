//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_2E13_44AD_B199_10ECE47A646E_HPP
#define POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_2E13_44AD_B199_10ECE47A646E_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirectInput.hpp"
#include "../Utility/Noncopyable.hpp"
#include <wrl/client.h>
#include <memory>

namespace Pomdog {
namespace Details {
namespace InputSystem {
namespace DirectInput {

class DeviceContextDirectInput final: Noncopyable {
public:
	DeviceContextDirectInput(HINSTANCE hInstance, HWND windowHandle);

	~DeviceContextDirectInput();

	HWND WindowHandle() const;

	IDirectInput8* GetDirectInput() const;

private:
	HWND windowHandle;
	Microsoft::WRL::ComPtr<IDirectInput8> directInput;
};

}// namespace DirectInput
}// namespace InputSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_2E13_44AD_B199_10ECE47A646E_HPP)
