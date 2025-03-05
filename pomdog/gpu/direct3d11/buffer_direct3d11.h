// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class BufferDirect3D11 final : public Buffer {
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
    D3D11_MAP mapTypeForWriting_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> device,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        D3D11_BIND_FLAG bindFlag) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> device,
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        D3D11_BIND_FLAG bindFlag) noexcept;

    void getData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void setData(
        std::size_t offsetInBytes,
        const void* sourceData,
        std::size_t sizeInBytes) override;

    /// Gets the pointer of the native buffer.
    [[nodiscard]] unsafe_ptr<ID3D11Buffer>
    getBuffer() const noexcept;
};

} // namespace pomdog::gpu::detail::direct3d11
