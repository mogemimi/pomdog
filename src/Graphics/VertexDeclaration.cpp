//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/VertexDeclaration.hpp>
#include <algorithm>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/detail/VertexElementHelper.hpp>

namespace Pomdog {
namespace {

static std::uint32_t AccumulateStrideBytes(std::vector<VertexElement> const& elements)
{
	std::uint32_t strideBytes = 0;
	for (auto & vertexElement: elements)
	{
		std::uint32_t currentStride = vertexElement.Offset
			+ Details::VertexElementHelper::ToByteSize(vertexElement.VertexElementFormat);
		if (strideBytes < currentStride) {
			strideBytes = currentStride;
		}
	}
	return strideBytes;
}

}// unnamed namespace
//-----------------------------------------------------------------------
VertexDeclaration::VertexDeclaration(std::vector<VertexElement> && vertexElements)
	: elements(std::move(vertexElements))
	, strideBytes(AccumulateStrideBytes(elements))
{
	POMDOG_ASSERT(!elements.empty());
	POMDOG_ASSERT(strideBytes > 0);
}
//-----------------------------------------------------------------------
VertexDeclaration::VertexDeclaration(std::initializer_list<VertexElement> vertexElements)
	: elements(vertexElements)
	, strideBytes(AccumulateStrideBytes(elements))
{
	POMDOG_ASSERT(!elements.empty());
	POMDOG_ASSERT(strideBytes > 0);
}
//-----------------------------------------------------------------------
VertexDeclaration::VertexDeclaration(std::vector<VertexElement> const& vertexElements)
	: elements(vertexElements)
	, strideBytes(AccumulateStrideBytes(elements))
{
	POMDOG_ASSERT(!elements.empty());
	POMDOG_ASSERT(strideBytes > 0);
}
//-----------------------------------------------------------------------
VertexDeclaration::VertexDeclaration(VertexElement const* vertexElements, std::size_t elementCount)
	: strideBytes(0)
{
	POMDOG_ASSERT(vertexElements != nullptr);
	POMDOG_ASSERT(elementCount > 0);

	for (std::size_t i = 0; i < elementCount; ++i) {
		this->elements.push_back(vertexElements[i]);
	}

	POMDOG_ASSERT(!elements.empty());

	strideBytes = AccumulateStrideBytes(elements);
	POMDOG_ASSERT(strideBytes > 0);
}
//-----------------------------------------------------------------------
std::vector<VertexElement> const& VertexDeclaration::VertexElements() const
{
	return elements;
}
//-----------------------------------------------------------------------
std::uint32_t VertexDeclaration::StrideBytes() const
{
	return strideBytes;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
