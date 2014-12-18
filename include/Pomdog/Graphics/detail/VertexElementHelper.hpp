//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_VERTEXELEMENTHELPER_8D22B0DB_DF29_466C_BD6B_7A095650D28F_HPP
#define POMDOG_VERTEXELEMENTHELPER_8D22B0DB_DF29_466C_BD6B_7A095650D28F_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "ForwardDeclarations.hpp"
#include "Pomdog/Config/Export.hpp"
#include <cstdint>

namespace Pomdog {
namespace Details {
namespace VertexElementHelper {

std::uint8_t POMDOG_EXPORT ToByteSize(VertexElementFormat format);

}// namespace VertexElementHelper
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXELEMENTHELPER_8D22B0DB_DF29_466C_BD6B_7A095650D28F_HPP)
