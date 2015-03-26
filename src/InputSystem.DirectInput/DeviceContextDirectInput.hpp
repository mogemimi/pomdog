// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_2E13_44AD_B199_10ECE47A646E_HPP
#define POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_2E13_44AD_B199_10ECE47A646E_HPP

#include "PrerequisitesDirectInput.hpp"
#include "../Utility/Noncopyable.hpp"
#include <wrl/client.h>
#include <memory>

namespace Pomdog {
namespace Detail {
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
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_DEVICECONTEXTDIRECTINPUT_A3771E91_2E13_44AD_B199_10ECE47A646E_HPP)
