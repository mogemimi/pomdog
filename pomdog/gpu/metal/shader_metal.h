// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/shader.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLLibrary.h>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
struct ShaderCompileOptions;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::metal {

/// ShaderMetal is the Metal implementation of Shader.
class ShaderMetal final : public Shader {
private:
    id<MTLFunction> shader_ = nullptr;

public:
    ~ShaderMetal() override;

    /// Creates a Metal function from precompiled bytecode embedded in the default library.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        id<MTLDevice> device,
        std::span<const u8> shaderBytecode,
        const ShaderCompileOptions& compileOptions) noexcept;

    /// Creates a Metal function from an existing MTLLibrary.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        id<MTLDevice> device,
        id<MTLLibrary> library,
        const ShaderCompileOptions& compileOptions) noexcept;

    /// Gets the pointer of the native shader resource.
    [[nodiscard]] id<MTLFunction>
    getShader() const noexcept;
};

} // namespace pomdog::gpu::detail::metal
