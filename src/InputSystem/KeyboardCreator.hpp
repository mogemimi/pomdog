// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_KEYBOARDCREATOR_E5930C37_HPP
#define POMDOG_KEYBOARDCREATOR_E5930C37_HPP

#include "InputDeviceCreator.hpp"

namespace Pomdog {

class Keyboard;

namespace Detail {
namespace InputSystem {

using KeyboardCreator = InputDeviceCreator<Keyboard>;

}// namespace InputSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_KEYBOARDCREATOR_E5930C37_HPP
