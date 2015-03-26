// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VERTEXELEMENT_1A25E253_HPP
#define POMDOG_VERTEXELEMENT_1A25E253_HPP

#include "VertexElementFormat.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT VertexElement {
public:
    std::uint16_t Offset;

    VertexElementFormat VertexElementFormat;
};

}// namespace Pomdog

#endif // POMDOG_VERTEXELEMENT_1A25E253_HPP
