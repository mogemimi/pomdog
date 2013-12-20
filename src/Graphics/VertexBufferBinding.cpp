//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/VertexBufferBinding.hpp>
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(VertexDeclaration const& vertexDeclarationIn)
: Declaration(vertexDeclarationIn)
, VertexOffset(0)
, InstanceFrequency(0)
{}
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(VertexDeclaration && vertexDeclarationIn)
: Declaration(std::move(vertexDeclarationIn))
, VertexOffset(0)
, InstanceFrequency(0)
{}
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(VertexDeclaration const& vertexDeclarationIn,
	std::uint32_t vertexOffsetIn)
: Declaration(vertexDeclarationIn)
, VertexOffset(vertexOffsetIn)
, InstanceFrequency(0)
{}
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(VertexDeclaration const& vertexDeclarationIn,
	std::uint32_t vertexOffsetIn, std::uint32_t instanceFrequencyIn)
: Declaration(vertexDeclarationIn)
, VertexOffset(vertexOffsetIn)
, InstanceFrequency(instanceFrequencyIn)
{}
//-----------------------------------------------------------------------
}// namespace Pomdog
