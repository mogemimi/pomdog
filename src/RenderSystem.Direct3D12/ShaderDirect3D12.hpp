// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SHADERDIRECT3D12_E47B4982_HPP
#define POMDOG_SHADERDIRECT3D12_E47B4982_HPP

#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include <cstdint>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {

class ShaderDirect3D12 final : public Shader {
public:
    std::vector<std::uint8_t> CodeBlob;

    ShaderBytecode GetShaderBytecode() const
    {
        ShaderBytecode shaderBytecode;
        shaderBytecode.Code = CodeBlob.data();
        shaderBytecode.ByteLength = CodeBlob.size();
        return std::move(shaderBytecode);
    }
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_SHADERDIRECT3D12_E47B4982_HPP
