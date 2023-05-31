// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/input_layout_helper.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/input_element_format.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu {
namespace {

#if !defined(NDEBUG)
constexpr std::uint16_t MaxInputSlotCount = 16;
#endif

} // namespace

InputLayoutHelper& InputLayoutHelper::pushBack(InputElementFormat format)
{
    InputElement element;
    element.format = format;
    element.inputSlot = inputSlot_;
    element.inputSlotClass = slotClass_;
    element.instanceStepRate = instanceStepRate_;
    element.byteOffset = byteOffset_;

    byteOffset_ += detail::BufferHelper::ToByteSize(format);
    elements_.push_back(std::move(element));
    POMDOG_ASSERT_MESSAGE(element.byteOffset < byteOffset_, "Overflowing of unsigned int.");

    return *this;
}

InputLayoutHelper& InputLayoutHelper::addByte4()
{
    pushBack(InputElementFormat::Byte4);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::addFloat()
{
    pushBack(InputElementFormat::Float);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::addFloat2()
{
    pushBack(InputElementFormat::Float2);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::addFloat3()
{
    pushBack(InputElementFormat::Float3);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::addFloat4()
{
    pushBack(InputElementFormat::Float4);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::addInt4()
{
    pushBack(InputElementFormat::Int4);
    return *this;
}

InputLayoutHelper& InputLayoutHelper::addInputSlot()
{
    if (!elements_.empty()) {
        ++inputSlot_;
    }

    POMDOG_ASSERT(inputSlot_ >= 0);
    POMDOG_ASSERT(inputSlot_ < MaxInputSlotCount);

    instanceStepRate_ = 0;
    slotClass_ = InputClassification::InputPerVertex;
    byteOffset_ = 0;
    return *this;
}

InputLayoutHelper& InputLayoutHelper::addInputSlot(
    InputClassification slotClassIn, std::uint16_t instanceStepRateIn)
{
    POMDOG_ASSERT(instanceStepRateIn >= 0);
    POMDOG_ASSERT((instanceStepRateIn == 0) ||
                  (slotClassIn == InputClassification::InputPerInstance));

    if (!elements_.empty()) {
        ++inputSlot_;
    }

    POMDOG_ASSERT(inputSlot_ >= 0);
    POMDOG_ASSERT(inputSlot_ < MaxInputSlotCount);

    instanceStepRate_ = instanceStepRateIn;
    slotClass_ = slotClassIn;
    byteOffset_ = 0;
    return *this;
}

InputLayoutDescriptor InputLayoutHelper::createInputLayout()
{
    return InputLayoutDescriptor{elements_};
}

} // namespace pomdog::gpu
