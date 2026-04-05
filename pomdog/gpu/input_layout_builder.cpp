// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::InputLayoutBuilder {
namespace {

[[nodiscard]] u16
toByteSize(InputElementFormat format) noexcept
{
    return static_cast<u16>(detail::BufferHelper::toByteSize(format));
}

} // namespace

void addVertex(
    InputLayoutDesc& desc,
    u16 inputSlot,
    InputClassification slotClass,
    u32 strideInBytes,
    std::initializer_list<InputElementFormat>&& elements) noexcept
{
    InputBufferLayoutDesc vertexBufferLayout;
    vertexBufferLayout.inputSlot = inputSlot;
    vertexBufferLayout.inputSlotClass = slotClass;
    vertexBufferLayout.strideBytes = strideInBytes;

    switch (slotClass) {
    case InputClassification::PerVertex:
        vertexBufferLayout.instanceStepRate = 0;
        break;
    case InputClassification::PerInstance:
        vertexBufferLayout.instanceStepRate = 1;
        break;
    }

    vertexBufferLayout.elements.reserve(vertexBufferLayout.elements.size() + elements.size());

    u16 byteOffset = 0;
    for (const auto& format : elements) {
        InputElementDesc element;
        element.byteOffset = byteOffset;
        element.format = format;

        byteOffset += toByteSize(format);

        // NOTE: detect overflow
        POMDOG_ASSERT(element.byteOffset < byteOffset);

        vertexBufferLayout.elements.push_back(std::move(element));
    }

    POMDOG_ASSERT(byteOffset <= strideInBytes);
    desc.vertexBuffers.push_back(std::move(vertexBufferLayout));
}

} // namespace pomdog::gpu::InputLayoutBuilder
