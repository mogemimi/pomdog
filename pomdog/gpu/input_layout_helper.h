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
    InputLayoutHelper& PushBack(InputElementFormat format);

    InputLayoutHelper& Byte4();

    InputLayoutHelper& Float();

    InputLayoutHelper& Float2();

    InputLayoutHelper& Float3();

    InputLayoutHelper& Float4();

    InputLayoutHelper& Int4();

    InputLayoutHelper& AddInputSlot();

    InputLayoutHelper& AddInputSlot(
        InputClassification slotClass,
        std::uint16_t instanceStepRate);

    InputLayoutDescriptor CreateInputLayout();

private:
    std::vector<InputElement> elements;
    std::uint16_t inputSlot = 0;
    std::uint16_t byteOffset = 0;
    std::uint16_t instanceStepRate = 0;
    InputClassification slotClass = InputClassification::InputPerVertex;
};

} // namespace pomdog::gpu
