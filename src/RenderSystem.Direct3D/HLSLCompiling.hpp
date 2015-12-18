// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D.hpp"
#include <wrl/client.h>
#include <cstdint>

namespace Pomdog {
namespace Detail {

class ShaderBytecode;
struct ShaderCompileOptions;

namespace Direct3D {

struct HLSLCompiling {
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        ShaderBytecode const& shaderBytecode,
        ShaderCompileOptions const& compileOptions);
};

} // namespace Direct3D
} // namespace Detail
} // namespace Pomdog
