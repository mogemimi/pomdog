// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INPUTELEMENT_E9D07191_HPP
#define POMDOG_INPUTELEMENT_E9D07191_HPP

#include "InputClassification.hpp"
#include "InputElementFormat.hpp"
#include <cstdint>

namespace Pomdog {

class InputElement {
public:
    std::uint16_t ByteOffset;

    std::uint16_t BufferIndex;

    InputElementFormat Format;

    InputClassification InputSlotClass;

    std::uint16_t InstanceStepRate;
};

} // namespace Pomdog

#endif // POMDOG_INPUTELEMENT_E9D07191_HPP
