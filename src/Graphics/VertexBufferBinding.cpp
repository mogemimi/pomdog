//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/VertexBufferBinding.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(
	std::shared_ptr<Pomdog::VertexBuffer> const& vertexBufferIn)
: VertexBuffer(vertexBufferIn)
, VertexOffset(0)
, InstanceFrequency(0)
{}
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(
	std::shared_ptr<Pomdog::VertexBuffer> const& vertexBufferIn, std::uint32_t vertexOffsetIn)
: VertexBuffer(vertexBufferIn)
, VertexOffset(vertexOffsetIn)
, InstanceFrequency(0)
{}
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(
	std::shared_ptr<Pomdog::VertexBuffer> const& vertexBufferIn, std::uint32_t vertexOffsetIn, std::uint32_t instanceFrequencyIn)
: VertexBuffer(vertexBufferIn)
, VertexOffset(vertexOffsetIn)
, InstanceFrequency(instanceFrequencyIn)
{}
//-----------------------------------------------------------------------
}// namespace Pomdog
