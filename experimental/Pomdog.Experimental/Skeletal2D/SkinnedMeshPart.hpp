// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKINNEDMESHPART_DAE847CA_8830_401A_8785_0EAB6DC603D5_HPP
#define POMDOG_SKINNEDMESHPART_DAE847CA_8830_401A_8785_0EAB6DC603D5_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

class SkinnedMeshPart {
public:
	std::uint16_t VertexCount;

	std::uint16_t VertexOffset;

	std::uint16_t PrimitiveCount;

	std::uint16_t StartIndex;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKINNEDMESHPART_DAE847CA_8830_401A_8785_0EAB6DC603D5_HPP)
