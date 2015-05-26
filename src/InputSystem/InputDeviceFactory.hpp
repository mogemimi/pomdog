// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INPUTDEVICEFACTORY_60E2AEA5_HPP
#define POMDOG_INPUTDEVICEFACTORY_60E2AEA5_HPP

#include "../Utility/Noncopyable.hpp"
#include <memory>

namespace Pomdog {

class Keyboard;
class Mouse;

namespace Detail {
namespace InputSystem {

class InputDeviceFactory final: Noncopyable {
public:
    ~InputDeviceFactory();

    //std::shared_ptr<Gamepad> CreateGamepad(SubsystemScheduler & scheduler);

private:
};

} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_INPUTDEVICEFACTORY_60E2AEA5_HPP
