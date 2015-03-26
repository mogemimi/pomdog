// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VERTEXDECLARATION_5FD2E1E3_HPP
#define POMDOG_VERTEXDECLARATION_5FD2E1E3_HPP

#include "VertexElement.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <vector>
#include <initializer_list>

namespace Pomdog {

class POMDOG_EXPORT VertexDeclaration {
public:
    VertexDeclaration(std::initializer_list<VertexElementFormat> && vertexElements);

    VertexDeclaration(std::initializer_list<VertexElement> && vertexElements);

    explicit VertexDeclaration(std::vector<VertexElement> const& vertexElements);

    explicit VertexDeclaration(std::vector<VertexElement> && vertexElements);

    VertexDeclaration(VertexElement const* vertexElements, std::size_t elementCount);

    std::uint32_t StrideBytes() const;

    std::vector<VertexElement> const& VertexElements() const;

private:
    std::vector<VertexElement> elements;

    std::uint32_t strideBytes;
};

}// namespace Pomdog

#endif // POMDOG_VERTEXDECLARATION_5FD2E1E3_HPP
