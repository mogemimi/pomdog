// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/direct3d11/prerequisites_direct3d11.hpp"
#include "pomdog/graphics/effect_reflection.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {
class ShaderBytecode;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Direct3D11 {

class EffectReflectionDirect3D11 final : public EffectReflection {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        const ShaderBytecode& vertexShaderBytecode,
        const ShaderBytecode& pixelShaderBytecode) noexcept;

    std::vector<EffectConstantDescription>
    GetConstantBuffers() const noexcept override;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> vertexShaderReflector;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pixelShaderReflector;
};

} // namespace Pomdog::Detail::Direct3D11
