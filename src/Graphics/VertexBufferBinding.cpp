// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(VertexDeclaration const& vertexDeclarationIn)
: Declaration(vertexDeclarationIn)
, VertexOffset(0)
, InstanceStepRate(0)
{}
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(VertexDeclaration && vertexDeclarationIn)
: Declaration(std::move(vertexDeclarationIn))
, VertexOffset(0)
, InstanceStepRate(0)
{}
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(VertexDeclaration const& vertexDeclarationIn,
    std::uint32_t vertexOffsetIn, std::uint32_t stepRateIn)
: Declaration(vertexDeclarationIn)
, VertexOffset(vertexOffsetIn)
, InstanceStepRate(stepRateIn)
{}
//-----------------------------------------------------------------------
VertexBufferBinding::VertexBufferBinding(VertexDeclaration && vertexDeclarationIn,
    std::uint32_t vertexOffsetIn, std::uint32_t stepRateIn)
: Declaration(std::move(vertexDeclarationIn))
, VertexOffset(vertexOffsetIn)
, InstanceStepRate(stepRateIn)
{}
//-----------------------------------------------------------------------
}// namespace Pomdog
