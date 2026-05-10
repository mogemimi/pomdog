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

/// Appends a vertex buffer layout to `desc`.
///
/// `inputSlot` is the pipeline input slot index; `slotClass` specifies per-vertex
/// or per-instance feeding; `strideInBytes` is the byte stride of one vertex;
/// `elements` lists the format of each input attribute within that stride.
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
/// Returns nullptr on success, or an Error describing the problem.
[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
validate(const InputLayoutDesc& desc) noexcept;

} // namespace pomdog::gpu::InputLayoutBuilder
