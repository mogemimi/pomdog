// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/graphics/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/sampler_state.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::direct3d11 {

class SamplerStateDirect3D11 final : public SamplerState {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        ID3D11Device* device,
        const SamplerDescription& description) noexcept;

    /// Gets the pointer of the native sampler state.
    [[nodiscard]] ID3D11SamplerState* GetSamplerState() const noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

} // namespace pomdog::detail::direct3d11
