// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <initializer_list>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
struct InputLayoutDesc;
enum class InputClassification : u8;
enum class InputElementFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::InputLayoutBuilder {

/// Adds a vertex buffer layout to the input layout description.
///
/// @param desc The input layout description to add the vertex buffer layout to.
/// @param inputSlot The index of the input slot.
/// @param slotClass Specifies whether the input data is per-vertex or per-instance.
/// @param strideInBytes The size in bytes of per-vertex data.
/// @param elements The list of input element formats.
POMDOG_EXPORT void
addVertex(
    InputLayoutDesc& desc,
    u16 inputSlot,
    InputClassification slotClass,
    u32 strideInBytes,
    std::initializer_list<InputElementFormat>&& elements) noexcept;

} // namespace pomdog::gpu::InputLayoutBuilder
