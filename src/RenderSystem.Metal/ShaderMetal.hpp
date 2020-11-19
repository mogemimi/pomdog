// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#import <Metal/MTLLibrary.h>

namespace Pomdog::Detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Metal {

class ShaderMetal final : public Shader {
public:
    ShaderMetal(
        id<MTLDevice> device,
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions);

    ShaderMetal(
        id<MTLDevice> device,
        id<MTLLibrary> library,
        const ShaderCompileOptions& compileOptions);

    id<MTLFunction> GetShader() const;

private:
    id<MTLFunction> shader = nullptr;
};

} // namespace Pomdog::Detail::Metal
