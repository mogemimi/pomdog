﻿//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/VertexDeclaration.hpp>
#include <algorithm>
#include <Pomdog/Utility/Assert.hpp>
#include "../Utility/HashingHelper.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
namespace {

static std::uint16_t ToByteSize(VertexElementFormat format)
{
	static_assert(sizeof(float) == 4, "FUS RO DAH");
	switch (format)
	{
	case VertexElementFormat::Float:
	case VertexElementFormat::HalfFloat2:
	case VertexElementFormat::Byte4:
		return 4;
	case VertexElementFormat::Float2:
	case VertexElementFormat::HalfFloat4:			
		return 8;
	case VertexElementFormat::Float3:
		return 12;
	case VertexElementFormat::Float4:
		return 16;
	}
#ifdef _MSC_VER
	POMDOG_ASSERT("cannot find format.");
	return 0; // error
#endif
}
//-----------------------------------------------------------------------
static std::uint32_t AccumulateStrideBytes(std::vector<VertexElement> const& elements)
{
	std::uint32_t strideBytes = 0;
	for (auto & vertexElement: elements)
	{
		std::uint32_t currentStride = vertexElement.Offset + ToByteSize(vertexElement.VertexElementFormat);
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
	this->internalHashKey = HashingHelper::Hash(elements.data(), elements.size());
}
//-----------------------------------------------------------------------
VertexDeclaration::VertexDeclaration(std::initializer_list<VertexElement> vertexElements)
	: elements(vertexElements)
	, strideBytes(AccumulateStrideBytes(elements))
{
	POMDOG_ASSERT(!elements.empty());
	this->internalHashKey = HashingHelper::Hash(elements.data(), elements.size());
}
//-----------------------------------------------------------------------
VertexDeclaration::VertexDeclaration(std::vector<VertexElement> const& vertexElements)
	: elements(vertexElements)
	, strideBytes(AccumulateStrideBytes(elements))
{
	POMDOG_ASSERT(!elements.empty());
	this->internalHashKey = HashingHelper::Hash(elements.data(), elements.size());
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
	this->internalHashKey = HashingHelper::Hash(elements.data(), elements.size());
}
//-----------------------------------------------------------------------
std::vector<VertexElement> const& VertexDeclaration::GetVertexElements() const
{
	return elements;
}
//-----------------------------------------------------------------------
std::uint32_t VertexDeclaration::GetStrideBytes() const
{
	return strideBytes;
}
//-----------------------------------------------------------------------
std::uint32_t VertexDeclaration::GetInternalHashKey() const
{
	return internalHashKey;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
