// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_HLSLCOMPILING_C56F5C9E_HPP
#define POMDOG_HLSLCOMPILING_C56F5C9E_HPP

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

#endif // POMDOG_HLSLCOMPILING_C56F5C9E_HPP
