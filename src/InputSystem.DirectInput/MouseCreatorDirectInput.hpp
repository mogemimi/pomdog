// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MOUSECREATORDIRECTINPUT_62BF491B_HPP
#define POMDOG_MOUSECREATORDIRECTINPUT_62BF491B_HPP

#include "../InputSystem/MouseCreator.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace DirectInput {

class DeviceContextDirectInput;

class MouseCreatorDirectInput final: public MouseCreator {
public:
    explicit MouseCreatorDirectInput(std::shared_ptr<DeviceContextDirectInput> const& deviceContext);

    std::shared_ptr<Mouse> Create(SubsystemScheduler & scheduler) override;

private:
    std::shared_ptr<DeviceContextDirectInput> deviceContext;
};

}// namespace DirectInput
}// namespace InputSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_MOUSECREATORDIRECTINPUT_62BF491B_HPP
