// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_MOUSECREATOR_CE792896_HPP
#define POMDOG_MOUSECREATOR_CE792896_HPP

#include "InputDeviceCreator.hpp"

namespace Pomdog {

class Mouse;

namespace Detail {
namespace InputSystem {

using MouseCreator = InputDeviceCreator<Mouse>;

}// namespace InputSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_MOUSECREATOR_CE792896_HPP
