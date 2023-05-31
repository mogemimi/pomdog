// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/input_element.h"
#include "pomdog/gpu/input_element_format.h"
#include "pomdog/gpu/input_layout_descriptor.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

class POMDOG_EXPORT InputLayoutHelper final {
public:
    InputLayoutHelper& pushBack(InputElementFormat format);

    InputLayoutHelper& addByte4();

    InputLayoutHelper& addFloat();

    InputLayoutHelper& addFloat2();

    InputLayoutHelper& addFloat3();

    InputLayoutHelper& addFloat4();

    InputLayoutHelper& addInt4();

    InputLayoutHelper& addInputSlot();

    InputLayoutHelper& addInputSlot(
        InputClassification slotClass,
        std::uint16_t instanceStepRate);

    InputLayoutDescriptor createInputLayout();

private:
    std::vector<InputElement> elements_;
    std::uint16_t inputSlot_ = 0;
    std::uint16_t byteOffset_ = 0;
    std::uint16_t instanceStepRate_ = 0;
    InputClassification slotClass_ = InputClassification::InputPerVertex;
};

} // namespace pomdog::gpu
