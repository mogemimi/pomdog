// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_KEYBOARDCREATORDIRECTINPUT_9AFA1F66_HPP
#define POMDOG_KEYBOARDCREATORDIRECTINPUT_9AFA1F66_HPP

#include "../InputSystem/KeyboardCreator.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace DirectInput {

class DeviceContextDirectInput;

class KeyboardCreatorDirectInput final: public KeyboardCreator {
public:
    explicit KeyboardCreatorDirectInput(std::shared_ptr<DeviceContextDirectInput> const& deviceContext);

    std::shared_ptr<Keyboard> Create(SubsystemScheduler & scheduler) override;

private:
    std::shared_ptr<DeviceContextDirectInput> deviceContext;
};

}// namespace DirectInput
}// namespace InputSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_KEYBOARDCREATORDIRECTINPUT_9AFA1F66_HPP
