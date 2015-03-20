//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_MOUSECREATOR_CE792896_2613_477F_A03D_327BE3F1A650_HPP
#define POMDOG_MOUSECREATOR_CE792896_2613_477F_A03D_327BE3F1A650_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "InputDeviceCreator.hpp"

namespace Pomdog {

class Mouse;

namespace Detail {
namespace InputSystem {

using MouseCreator = InputDeviceCreator<Mouse>;

}// namespace InputSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_MOUSECREATOR_CE792896_2613_477F_A03D_327BE3F1A650_HPP)
