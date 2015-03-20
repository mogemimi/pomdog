//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Graphics/VertexDeclaration.hpp"
#include "Pomdog/Graphics/detail/VertexElementHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace {

static std::uint32_t AccumulateStrideBytes(std::vector<VertexElement> const& elements)
{
	std::uint32_t strideBytes = 0;
	for (auto & vertexElement: elements)
	{
		std::uint32_t currentStride = vertexElement.Offset
			+ Detail::VertexElementHelper::ToByteSize(vertexElement.VertexElementFormat);
		if (strideBytes < currentStride) {
			strideBytes = currentStride;
		}
	}
	return strideBytes;
}
//-----------------------------------------------------------------------
static std::vector<VertexElement> BuildVertexElements(std::initializer_list<VertexElementFormat> && formats)
{
	std::vector<VertexElement> vertexElements;
	vertexElements.reserve(formats.size());

	std::uint16_t offsetBytes = 0;

	for (auto & format: formats)
	{
		vertexElements.push_back({offsetBytes, format});
		offsetBytes += Detail::VertexElementHelper::ToByteSize(format);
	}

	return std::move(vertexElements);
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
VertexDeclaration::VertexDeclaration(std::initializer_list<VertexElementFormat> && vertexElements)
	: VertexDeclaration(BuildVertexElements(std::move(vertexElements)))
{}
//-----------------------------------------------------------------------
VertexDeclaration::VertexDeclaration(std::initializer_list<VertexElement> && vertexElements)
	: elements(std::move(vertexElements))
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
