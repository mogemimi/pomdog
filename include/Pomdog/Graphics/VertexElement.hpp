// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VERTEXELEMENT_1A25E253_DA05_4C23_AEBD_4D04882E23E0_HPP
#define POMDOG_VERTEXELEMENT_1A25E253_DA05_4C23_AEBD_4D04882E23E0_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

#endif // !defined(POMDOG_VERTEXELEMENT_1A25E253_DA05_4C23_AEBD_4D04882E23E0_HPP)
