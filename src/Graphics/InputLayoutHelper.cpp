// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "../Graphics.Backends/BufferHelper.hpp"
#include "Pomdog/Graphics/InputElementFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace {

#ifdef DEBUG
static constexpr std::uint16_t MaxInputSlotCount = 16;
#endif

} // namespace

InputLayoutHelper& InputLayoutHelper::PushBack(InputElementFormat format)
{
    InputElement element;
    element.Format = format;
    element.InputSlot = inputSlot;
    element.InputSlotClass = slotClass;
    element.InstanceStepRate = instanceStepRate;
    element.ByteOffset = byteOffset;

    byteOffset += Detail::BufferHelper::ToByteSize(format);
    elements.push_back(std::move(element));
    POMDOG_ASSERT_MESSAGE(element.ByteOffset < byteOffset, "Overflowing of unsigned int.");

    return *this;
}

InputLayoutHelper& InputLayoutHelper::Byte4()
{
    PushBack(InputElementFormat::Byte4);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::Float()
{
    PushBack(InputElementFormat::Float);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::Float2()
{
    PushBack(InputElementFormat::Float2);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::Float3()
{
    PushBack(InputElementFormat::Float3);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::Float4()
{
    PushBack(InputElementFormat::Float4);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::Int4()
{
    PushBack(InputElementFormat::Int4);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::AddInputSlot()
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

InputLayoutHelper& InputLayoutHelper::AddInputSlot(
    InputClassification slotClassIn, std::uint16_t instanceStepRateIn)
{
    POMDOG_ASSERT(instanceStepRateIn >= 0);
    POMDOG_ASSERT((instanceStepRateIn == 0) ||
                  (slotClassIn == InputClassification::InputPerInstance));

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

InputLayoutDescription InputLayoutHelper::CreateInputLayout()
{
    return InputLayoutDescription{elements};
}

} // namespace Pomdog
