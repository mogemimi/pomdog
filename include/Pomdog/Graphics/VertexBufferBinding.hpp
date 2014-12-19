//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_VERTEXBUFFERBINDING_3055CF20_F4BD_4372_B6C5_8A279BA46082_HPP
#define POMDOG_VERTEXBUFFERBINDING_3055CF20_F4BD_4372_B6C5_8A279BA46082_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "VertexDeclaration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT VertexBufferBinding {
public:
	VertexDeclaration Declaration;
	std::uint32_t VertexOffset;
	std::uint32_t InstanceFrequency;
	
	VertexBufferBinding(VertexDeclaration const& vertexDeclaration);
	
	VertexBufferBinding(VertexDeclaration && vertexDeclaration);
	
	VertexBufferBinding(VertexDeclaration const& vertexDeclaration,
		std::uint32_t vertexOffset, std::uint32_t instanceFrequency = 0);

	VertexBufferBinding(VertexDeclaration && vertexDeclaration,
		std::uint32_t vertexOffset, std::uint32_t instanceFrequency = 0);
};

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXBUFFERBINDING_3055CF20_F4BD_4372_B6C5_8A279BA46082_HPP)
