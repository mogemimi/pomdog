// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/EffectReflection.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"

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
