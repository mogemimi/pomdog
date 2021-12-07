// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/input_classification.hpp"
#include "pomdog/graphics/input_element_format.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct InputElement final {
    std::uint16_t ByteOffset;
    std::uint16_t InputSlot;
    InputElementFormat Format;
    InputClassification InputSlotClass;
    std::uint16_t InstanceStepRate;
};

} // namespace Pomdog
