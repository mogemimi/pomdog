// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/InputElement.hpp"
#include "Pomdog/Graphics/InputElementFormat.hpp"
#include "Pomdog/Graphics/InputLayoutDescription.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstdint>

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
