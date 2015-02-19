//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_CUSTOMVERTEX_614B90BE_45A7_486E_909C_2A51C33975C8_HPP
#define POMDOG_CUSTOMVERTEX_614B90BE_45A7_486E_909C_2A51C33975C8_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/VertexElementHelper.hpp"
#include "VertexDeclaration.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <cstddef>
#include <array>
#include <utility>
#include <vector>
#include <initializer_list>

namespace Pomdog {
namespace Details {
namespace Graphics {

template <typename T> struct ToVertexElementFormat;
template <> struct ToVertexElementFormat<float> {
	static constexpr VertexElementFormat value = VertexElementFormat::Float;
};
template <> struct ToVertexElementFormat<Vector2> {
	static constexpr VertexElementFormat value = VertexElementFormat::Float2;
};
template <> struct ToVertexElementFormat<Vector3> {
	static constexpr VertexElementFormat value = VertexElementFormat::Float3;
};
template <> struct ToVertexElementFormat<Vector4> {
	static constexpr VertexElementFormat value = VertexElementFormat::Float4;
};
template <> struct ToVertexElementFormat<Color> {
	static constexpr VertexElementFormat value = VertexElementFormat::Float4;
};
template <> struct ToVertexElementFormat<Quaternion> {
	static constexpr VertexElementFormat value = VertexElementFormat::Float4;
};
template <> struct ToVertexElementFormat<std::array<std::uint8_t, 4>> {
	static constexpr VertexElementFormat value = VertexElementFormat::Byte4;
};
template <> struct ToVertexElementFormat<std::uint8_t[4]> {
	static constexpr VertexElementFormat value = VertexElementFormat::Byte4;
};
template <> struct ToVertexElementFormat<std::int32_t[4]> {
	static constexpr VertexElementFormat value = VertexElementFormat::Int4;
};

template <VertexElementFormat... Formats>
std::vector<VertexElement> VertexCombined()
{
	std::initializer_list<VertexElementFormat> formats { Formats... };

	std::vector<VertexElement> vertexElements;
	vertexElements.reserve(formats.size());
	std::uint16_t offsetBytes = 0;
	for (auto format: formats) {
		vertexElements.push_back({offsetBytes, format});
		offsetBytes += VertexElementHelper::ToByteSize(format);
	}
	return std::move(vertexElements);
}

template <typename... ElementTypes>
struct VertexElementTuple;

template <typename Element>
struct VertexElementTuple<Element> {
	Element end;
};

template <typename Head, typename... ElementTypes>
struct VertexElementTuple<Head, ElementTypes...> {
	Head first;
	VertexElementTuple<ElementTypes...> elements;
};

}// namespace Graphics
}// namespace Details

template <typename T, typename... Arguments>
class CustomVertex {
public:
	Details::Graphics::VertexElementTuple<T, Arguments...> Tuple;

	static VertexDeclaration Declaration()
	{
		using Details::Graphics::VertexCombined;
		using Details::Graphics::ToVertexElementFormat;
		return VertexDeclaration(VertexCombined<
			ToVertexElementFormat<T>::value,
			ToVertexElementFormat<Arguments>::value...
		>());
	}
};

}// namespace Pomdog

#endif // !defined(POMDOG_CUSTOMVERTEX_614B90BE_45A7_486E_909C_2A51C33975C8_HPP)
