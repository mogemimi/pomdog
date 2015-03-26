// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VERTEXBUFFERBINDING_3055CF20_HPP
#define POMDOG_VERTEXBUFFERBINDING_3055CF20_HPP

#include "VertexDeclaration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT VertexBufferBinding final {
public:
    VertexDeclaration Declaration;
    std::uint32_t VertexOffset;
    std::uint32_t InstanceStepRate;

    VertexBufferBinding(VertexDeclaration const& vertexDeclaration);

    VertexBufferBinding(VertexDeclaration && vertexDeclaration);

    VertexBufferBinding(VertexDeclaration const& vertexDeclaration,
        std::uint32_t vertexOffset, std::uint32_t stepRate = 0);

    VertexBufferBinding(VertexDeclaration && vertexDeclaration,
        std::uint32_t vertexOffset, std::uint32_t stepRate = 0);
};

}// namespace Pomdog

#endif // POMDOG_VERTEXBUFFERBINDING_3055CF20_HPP
