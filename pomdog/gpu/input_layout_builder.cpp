// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/input_layout_desc.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <set>
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

std::unique_ptr<Error>
validate(const InputLayoutDesc& desc) noexcept
{
    if (desc.vertexBuffers.empty()) {
        return errors::make("InputLayoutDesc has no vertex buffers");
    }

    std::set<u16> seenSlots = {};

    for (const auto& buffer : desc.vertexBuffers) {
        if (!seenSlots.insert(buffer.inputSlot).second) {
            return errors::make(
                "duplicate inputSlot " + std::to_string(buffer.inputSlot));
        }

        if (buffer.inputSlotClass == InputClassification::PerVertex &&
            buffer.instanceStepRate != 0) {
            return errors::make(
                "instanceStepRate must be 0 for per-vertex slot " +
                std::to_string(buffer.inputSlot));
        }

        if (buffer.elements.empty()) {
            return errors::make(
                "vertex buffer at slot " + std::to_string(buffer.inputSlot) +
                " has no elements");
        }

        u32 totalElementBytes = 0;
        for (const auto& element : buffer.elements) {
            totalElementBytes += toByteSize(element.format);
        }

        if (buffer.strideBytes < totalElementBytes) {
            return errors::make(
                "strideBytes (" + std::to_string(buffer.strideBytes) +
                ") is less than total element size (" +
                std::to_string(totalElementBytes) + ") at slot " +
                std::to_string(buffer.inputSlot));
        }
    }

    return nullptr;
}

} // namespace pomdog::gpu::InputLayoutBuilder
