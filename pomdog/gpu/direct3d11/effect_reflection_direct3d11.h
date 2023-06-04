// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/shader_reflections/effect_reflection.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail {
class ShaderBytecode;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::direct3d11 {

class EffectReflectionDirect3D11 final : public EffectReflection {
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> vertexShaderReflector_;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pixelShaderReflector_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const ShaderBytecode& vertexShaderBytecode,
        const ShaderBytecode& pixelShaderBytecode) noexcept;

    [[nodiscard]] std::vector<EffectConstantDescription>
    getConstantBuffers() const noexcept override;
};

} // namespace pomdog::gpu::detail::direct3d11
