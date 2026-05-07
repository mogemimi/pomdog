// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/buffer.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/MTLBuffer.h>
#include <memory>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
enum class MemoryUsage : u8;
struct BufferDesc;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail::metal {
class FrameCounter;
} // namespace pomdog::gpu::detail::metal

namespace pomdog::gpu::detail::metal {

class BufferMetal final : public Buffer {
private:
    std::vector<id<MTLBuffer>> buffers_;
    std::shared_ptr<const FrameCounter> frameCounter_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<const FrameCounter> frameCounter,
        id<MTLDevice> device,
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
    [[nodiscard]] id<MTLBuffer>
    getBuffer() const noexcept;
};

} // namespace pomdog::gpu::detail::metal
