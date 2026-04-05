// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <initializer_list>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

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

/// Validates the input layout description.
///
/// Checks that stride is consistent with element sizes, that there are no
/// duplicate input slots, and that instanceStepRate is zero for per-vertex data.
///
/// @param desc The input layout description to validate.
/// @returns nullptr on success, or an Error describing the problem.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
validate(const InputLayoutDesc& desc) noexcept;

} // namespace pomdog::gpu::InputLayoutBuilder
