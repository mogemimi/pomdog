//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_KEYBOARDCREATOR_E5930C37_6D5B_4529_A6DB_2F789D2CA82E_HPP
#define POMDOG_KEYBOARDCREATOR_E5930C37_6D5B_4529_A6DB_2F789D2CA82E_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "InputDeviceCreator.hpp"

namespace Pomdog {

class Keyboard;

namespace Details {
namespace InputSystem {

using KeyboardCreator = InputDeviceCreator<Keyboard>;

}// namespace InputSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_KEYBOARDCREATOR_E5930C37_6D5B_4529_A6DB_2F789D2CA82E_HPP)
