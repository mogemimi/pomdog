//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VERTEXBUFFERBINDING_3055CF20_F4BD_4372_B6C5_8A279BA46082_HPP
#define POMDOG_VERTEXBUFFERBINDING_3055CF20_F4BD_4372_B6C5_8A279BA46082_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "detail/ForwardDeclarations.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Graphics
/// @{

class VertexBufferBinding
{
public:
	std::shared_ptr<VertexBuffer> VertexBuffer;
	std::uint32_t VertexOffset;
	std::uint32_t InstanceFrequency;

	VertexBufferBinding(std::shared_ptr<Pomdog::VertexBuffer> const&);

	VertexBufferBinding(std::shared_ptr<Pomdog::VertexBuffer> const&, std::uint32_t vertexOffset);

	VertexBufferBinding(std::shared_ptr<Pomdog::VertexBuffer> const&, std::uint32_t vertexOffset, std::uint32_t instanceFrequency);
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_VERTEXBUFFERBINDING_3055CF20_F4BD_4372_B6C5_8A279BA46082_HPP)
