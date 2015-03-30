// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/InputElementFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

static std::uint8_t ToByteSize(InputElementFormat format)
{
    static_assert(sizeof(float) == 4, "FUS RO DAH");
    switch (format) {
    case InputElementFormat::Byte4:
    case InputElementFormat::Float:
    case InputElementFormat::HalfFloat2:
        return 4;
    case InputElementFormat::Float2:
    case InputElementFormat::HalfFloat4:
        return 8;
    case InputElementFormat::Float3:
        return 12;
    case InputElementFormat::Int4:
    case InputElementFormat::Float4:
        return 16;
    }
#ifdef _MSC_VER
    POMDOG_ASSERT("cannot find format.");
    return 4;
#endif
}

static constexpr std::uint16_t MaxInputSlotCount = 16;

} // unnamed namespace
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::PushBack(InputElementFormat format)
{
    InputElement element;
    element.Format = format;
    element.InputSlot = inputSlot;
    element.InputSlotClass = slotClass;
    element.InstanceStepRate = instanceStepRate;
    element.ByteOffset = byteOffset;

    byteOffset += ToByteSize(format);
    elements.push_back(std::move(element));

    return *this;
}
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::Byte4()
{
    PushBack(InputElementFormat::Byte4);
    return *this;
}
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::Float()
{
    PushBack(InputElementFormat::Float);
    return *this;
}
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::Float2()
{
    PushBack(InputElementFormat::Float2);
    return *this;
}
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::Float3()
{
    PushBack(InputElementFormat::Float3);
    return *this;
}
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::Float4()
{
    PushBack(InputElementFormat::Float4);
    return *this;
}
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::Int4()
{
    PushBack(InputElementFormat::Int4);
    return *this;
}
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::AddInputSlot()
{
    if (!elements.empty()) {
        ++inputSlot;
    }

    POMDOG_ASSERT(inputSlot >= 0);
    POMDOG_ASSERT(inputSlot < MaxInputSlotCount);

    instanceStepRate = 0;
    slotClass = InputClassification::InputPerVertex;
    byteOffset = 0;
    return *this;
}
//-----------------------------------------------------------------------
InputLayoutHelper & InputLayoutHelper::AddInputSlot(
    InputClassification slotClassIn, std::uint16_t instanceStepRateIn)
{
    POMDOG_ASSERT(instanceStepRateIn >= 0);
    POMDOG_ASSERT(instanceStepRateIn == 0 ||
        slotClassIn == InputClassification::InputPerInstance);

    if (!elements.empty()) {
        ++inputSlot;
    }

    POMDOG_ASSERT(inputSlot >= 0);
    POMDOG_ASSERT(inputSlot < MaxInputSlotCount);

    instanceStepRate = instanceStepRateIn;
    slotClass = slotClassIn;
    byteOffset = 0;
    return *this;
}
//-----------------------------------------------------------------------
InputLayoutDescription InputLayoutHelper::CreateInputLayout()
{
    return InputLayoutDescription{elements};
}
//-----------------------------------------------------------------------
} // namespace Pomdog
