// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <memory>
#include <utility>

namespace Pomdog {

struct POMDOG_EXPORT VertexBufferBinding final {
    std::shared_ptr<Pomdog::VertexBuffer> VertexBuffer;
    std::size_t VertexOffset;

    VertexBufferBinding() noexcept
        : VertexOffset(0)
    {
    }

    VertexBufferBinding(
        const std::shared_ptr<Pomdog::VertexBuffer>& vertexBufferIn) noexcept
        : VertexBufferBinding(vertexBufferIn, 0)
    {
    }

    VertexBufferBinding(
        std::shared_ptr<Pomdog::VertexBuffer>&& vertexBufferIn) noexcept
        : VertexBufferBinding(std::move(vertexBufferIn), 0)
    {
    }

    VertexBufferBinding(
        const std::shared_ptr<Pomdog::VertexBuffer>& vertexBufferIn,
        std::size_t offsetIn) noexcept
        : VertexBuffer(vertexBufferIn)
        , VertexOffset(offsetIn)
    {
    }

    VertexBufferBinding(
        std::shared_ptr<Pomdog::VertexBuffer>&& vertexBufferIn,
        std::size_t offsetIn) noexcept
        : VertexBuffer(std::move(vertexBufferIn))
        , VertexOffset(offsetIn)
    {
    }
};

} // namespace Pomdog
