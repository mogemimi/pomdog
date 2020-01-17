// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D.hpp"
#include <wrl/client.h>
#include <cstdint>

namespace Pomdog::Detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Direct3D {

struct HLSLCompiling final {
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions);
};

} // namespace Pomdog::Detail::Direct3D
