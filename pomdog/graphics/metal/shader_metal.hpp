// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/shader.hpp"
#include "pomdog/utility/errors.hpp"
#import <Metal/MTLLibrary.h>

namespace Pomdog::Detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Metal {

class ShaderMetal final : public Shader {
public:
    ~ShaderMetal() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        id<MTLDevice> device,
        id<MTLLibrary> library,
        const ShaderCompileOptions& compileOptions) noexcept;

    /// Gets the pointer of the native shader resource.
    id<MTLFunction> GetShader() const noexcept;

private:
    id<MTLFunction> shader = nullptr;
};

} // namespace Pomdog::Detail::Metal
