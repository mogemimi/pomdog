// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INPUTDEVICEFACTORY_60E2AEA5_DD8F_4F83_85A6_E364081B2E01_HPP
#define POMDOG_INPUTDEVICEFACTORY_60E2AEA5_DD8F_4F83_85A6_E364081B2E01_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "KeyboardCreator.hpp"
#include "MouseCreator.hpp"
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

    std::shared_ptr<Keyboard> CreateKeyboard(SubsystemScheduler & scheduler);

    std::shared_ptr<Mouse> CreateMouse(SubsystemScheduler & scheduler);

    void AddCreator(std::unique_ptr<KeyboardCreator> && creator);

    void AddCreator(std::unique_ptr<MouseCreator> && creator);

private:
    std::unique_ptr<KeyboardCreator> keyboardCreator;
    std::unique_ptr<MouseCreator> mouseCreator;
};

}// namespace InputSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_INPUTDEVICEFACTORY_60E2AEA5_DD8F_4F83_85A6_E364081B2E01_HPP)
