// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INPUTLAYOUTHELPER_A6C6ACE6_HPP
#define POMDOG_INPUTLAYOUTHELPER_A6C6ACE6_HPP

#include "detail/ForwardDeclarations.hpp"
#include "InputElementFormat.hpp"
#include "InputElement.hpp"
#include "InputLayoutDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT InputLayoutHelper final {
public:
    InputLayoutHelper & PushBack(InputElementFormat format);

    InputLayoutHelper & Byte4();

    InputLayoutHelper & Float();

    InputLayoutHelper & Float2();

    InputLayoutHelper & Float3();

    InputLayoutHelper & Float4();

    InputLayoutHelper & Int4();

    InputLayoutHelper & StartBuffer();

    InputLayoutHelper & StartBuffer(InputClassification slotClass,
        std::uint16_t instanceStepRate);

    InputLayoutDescription CreateInputLayout();

private:
    std::vector<InputElement> elements;
    std::uint16_t bufferIndex = 0;
    std::uint16_t byteOffset = 0;
    std::uint16_t instanceStepRate = 0;
    InputClassification slotClass = InputClassification::InputPerVertex;
};

} // namespace Pomdog

#endif // POMDOG_INPUTLAYOUTHELPER_A6C6ACE6_HPP
