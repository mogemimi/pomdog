// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/sampler_state.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class SamplerStateDirect3D11 final : public SamplerState {
private:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        ID3D11Device* device,
        const SamplerDescriptor& descriptor) noexcept;

    /// Gets the pointer of the native sampler state.
    [[nodiscard]] ID3D11SamplerState*
    getSamplerState() const noexcept;
};

} // namespace pomdog::gpu::detail::direct3d11
