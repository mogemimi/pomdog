// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/input_classification.h"
#include "pomdog/gpu/input_element_format.h"

namespace pomdog::gpu {

struct InputElement final {
    u16 byteOffset;
    u16 inputSlot;
    InputElementFormat format;
    InputClassification inputSlotClass;
    u16 instanceStepRate;
};

} // namespace pomdog::gpu
