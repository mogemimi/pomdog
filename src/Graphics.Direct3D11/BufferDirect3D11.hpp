// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "../Graphics.Backends/NativeBuffer.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Direct3D11 {

class BufferDirect3D11 final : public NativeBuffer {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        ID3D11Device* device,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        D3D11_BIND_FLAG bindFlag) noexcept;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        ID3D11Device* devuce,
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        D3D11_BIND_FLAG bindFlag) noexcept;

    void GetData(
        std::size_t offsetInBytes,
        void* destination,
        std::size_t sizeInBytes) const override;

    void SetData(
        std::size_t offsetInBytes,
        const void* sourceData,
        std::size_t sizeInBytes) override;

    /// Gets the pointer of the native buffer.
    [[nodiscard]] ID3D11Buffer* GetBuffer() const noexcept;

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    D3D11_MAP mapTypeForWriting;
};

} // namespace Pomdog::Detail::Direct3D11
