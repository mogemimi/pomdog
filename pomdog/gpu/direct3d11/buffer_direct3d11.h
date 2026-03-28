// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class MemoryUsage : u8;
struct BufferDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::direct3d11 {

class BufferDirect3D11 final : public Buffer {
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
    D3D11_MAP mapTypeForWriting_;
    MemoryUsage memoryUsage_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<ID3D11Device> device,
        const BufferDesc& desc,
        std::span<const u8> initialData) noexcept;

    void getData(
        u32 offsetInBytes,
        std::span<u8> destination) const override;

    void setData(
        u32 offsetInBytes,
        std::span<const u8> source) override;

    [[nodiscard]] std::span<u8>
    map(u32 offsetInBytes, u32 sizeInBytes) override;

    void unmap() override;

    /// Gets the pointer of the native buffer.
    [[nodiscard]] unsafe_ptr<ID3D11Buffer>
    getBuffer() const noexcept;
};

} // namespace pomdog::gpu::detail::direct3d11
