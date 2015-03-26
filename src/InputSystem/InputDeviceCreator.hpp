// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INPUTDEVICECREATOR_D5540F16_00DB_404F_BD2F_C24E6819474A_HPP
#define POMDOG_INPUTDEVICECREATOR_D5540F16_00DB_404F_BD2F_C24E6819474A_HPP

#include <memory>

namespace Pomdog {
namespace Detail {

class SubsystemScheduler;

namespace InputSystem {

template <class T>
class InputDeviceCreator {
public:
    virtual ~InputDeviceCreator() = default;

    virtual std::shared_ptr<T> Create(SubsystemScheduler & scheduler) = 0;
};

}// namespace InputSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_INPUTDEVICECREATOR_D5540F16_00DB_404F_BD2F_C24E6819474A_HPP)
