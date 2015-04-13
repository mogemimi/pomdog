// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERCOMPILING_C56F5C9E_HPP
#define POMDOG_SHADERCOMPILING_C56F5C9E_HPP

#include "PrerequisitesDirect3D11.hpp"
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <cstdint>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class ShaderBytecode;
class ShaderCompileOptions;

namespace Direct3D11 {

struct ShaderCompiling final {
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        ShaderBytecode const& shaderBytecode,
        ShaderCompileOptions const& compileOptions);
};

} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_SHADERCOMPILING_C56F5C9E_HPP
