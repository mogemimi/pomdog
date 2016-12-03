// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Metal/MTLLibrary.h>

namespace Pomdog {
namespace Detail {

class ShaderBytecode;
struct ShaderCompileOptions;

namespace Metal {

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
    id<MTLFunction> shader;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
