// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/input_element.hpp"
#include "pomdog/graphics/input_element_format.hpp"
#include "pomdog/graphics/input_layout_description.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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

    InputLayoutDescription CreateInputLayout();

private:
    std::vector<InputElement> elements;
    std::uint16_t inputSlot = 0;
    std::uint16_t byteOffset = 0;
    std::uint16_t instanceStepRate = 0;
    InputClassification slotClass = InputClassification::InputPerVertex;
};

} // namespace Pomdog
