// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include <Pomdog/Math/detail/ForwardDeclarations.hpp>
#include <Pomdog/Graphics/CustomVertex.hpp>
#include <Pomdog/Graphics/VertexDeclaration.hpp>
#include <Pomdog/Graphics/VertexElementFormat.hpp>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector4.hpp>
#include <Pomdog/Math/Quaternion.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::CustomVertex;
using Pomdog::VertexDeclaration;
using Pomdog::VertexElement;
using Pomdog::VertexElementFormat;
using Pomdog::Vector2;
using Pomdog::Vector3;
using Pomdog::Vector4;
using Pomdog::Quaternion;
using Pomdog::Color;

TEST(CustomVertex, Float)
{
	using VertexCombined = CustomVertex<float>;
	auto declaration = VertexCombined::Declaration();
	
	EXPECT_EQ(4, declaration.StrideBytes());
	ASSERT_EQ(1, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
}

TEST(CustomVertex, Vector2)
{
	using VertexCombined = CustomVertex<Vector2>;
	auto declaration = VertexCombined::Declaration();

	EXPECT_EQ(8, declaration.StrideBytes());
	ASSERT_EQ(1, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float2, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
}

TEST(CustomVertex, Vector3)
{
	using VertexCombined = CustomVertex<Vector3>;
	auto declaration = VertexCombined::Declaration();

	EXPECT_EQ(12, declaration.StrideBytes());
	ASSERT_EQ(1, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float3, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
}

TEST(CustomVertex, Vector4)
{
	using VertexCombined = CustomVertex<Vector4>;
	auto declaration = VertexCombined::Declaration();

	EXPECT_EQ(16, declaration.StrideBytes());
	ASSERT_EQ(1, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
}

TEST(CustomVertex, Color)
{
	using VertexCombined = CustomVertex<Color>;
	auto declaration = VertexCombined::Declaration();

	EXPECT_EQ(16, declaration.StrideBytes());
	ASSERT_EQ(1, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
}

TEST(CustomVertex, Quaternion)
{
	using VertexCombined = CustomVertex<Quaternion>;
	auto declaration = VertexCombined::Declaration();

	EXPECT_EQ(16, declaration.StrideBytes());
	ASSERT_EQ(1, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
}

TEST(CustomVertex, Vector4Two)
{
	using VertexCombined = CustomVertex<Vector4, Vector4>;
	auto declaration = VertexCombined::Declaration();

	EXPECT_EQ(16 * 2, declaration.StrideBytes());
	ASSERT_EQ(2, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[1].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
	EXPECT_EQ(16, declaration.VertexElements()[1].Offset);
}

TEST(CustomVertex, Vector4Three)
{
	using VertexCombined = CustomVertex<Vector4, Vector4, Vector4>;
	auto declaration = VertexCombined::Declaration();
	
	EXPECT_EQ(16 * 3, declaration.StrideBytes());
	ASSERT_EQ(3, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[1].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[2].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
	EXPECT_EQ(16, declaration.VertexElements()[1].Offset);
	EXPECT_EQ(32, declaration.VertexElements()[2].Offset);
}

TEST(CustomVertex, Vector4Four)
{
	using VertexCombined = CustomVertex<Vector4, Vector4, Vector4, Vector4>;
	auto declaration = VertexCombined::Declaration();
	
	EXPECT_EQ(16 * 4, declaration.StrideBytes());
	ASSERT_EQ(4, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[1].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[2].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[3].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
	EXPECT_EQ(16, declaration.VertexElements()[1].Offset);
	EXPECT_EQ(32, declaration.VertexElements()[2].Offset);
	EXPECT_EQ(48, declaration.VertexElements()[3].Offset);
}

TEST(CustomVertex, VectorTuple2)
{
	using VertexCombined = CustomVertex<Vector4, Vector3>;
	auto declaration = VertexCombined::Declaration();
	
	EXPECT_EQ(16+12, declaration.StrideBytes());
	ASSERT_EQ(2, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float3, declaration.VertexElements()[1].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
	EXPECT_EQ(16, declaration.VertexElements()[1].Offset);
}

TEST(CustomVertex, VectorTuple3)
{
	using VertexCombined = CustomVertex<Vector4, Vector3, Vector2>;
	auto declaration = VertexCombined::Declaration();
	
	EXPECT_EQ(16+12+8, declaration.StrideBytes());
	ASSERT_EQ(3, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float3, declaration.VertexElements()[1].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float2, declaration.VertexElements()[2].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
	EXPECT_EQ(16, declaration.VertexElements()[1].Offset);
	EXPECT_EQ(16+12, declaration.VertexElements()[2].Offset);
}

TEST(CustomVertex, VectorTuple4)
{
	using VertexCombined = CustomVertex<Vector4, Vector3, Vector2, float>;
	auto declaration = VertexCombined::Declaration();
	
	EXPECT_EQ(16+12+8+4, declaration.StrideBytes());
	ASSERT_EQ(4, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float3, declaration.VertexElements()[1].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float2, declaration.VertexElements()[2].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float,  declaration.VertexElements()[3].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
	EXPECT_EQ(16, declaration.VertexElements()[1].Offset);
	EXPECT_EQ(16+12, declaration.VertexElements()[2].Offset);
	EXPECT_EQ(16+12+8, declaration.VertexElements()[3].Offset);
}

TEST(CustomVertex, VectorTuple8)
{
	using VertexCombined = CustomVertex<
		Vector4, Vector3, Vector2, float,
		Vector4, Vector3, Vector2, float
	>;
	auto declaration = VertexCombined::Declaration();
	
	EXPECT_EQ(16+12+8+4+16+12+8+4, declaration.StrideBytes());
	ASSERT_EQ(8, declaration.VertexElements().size());
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[0].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float3, declaration.VertexElements()[1].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float2, declaration.VertexElements()[2].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float,  declaration.VertexElements()[3].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float4, declaration.VertexElements()[4].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float3, declaration.VertexElements()[5].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float2, declaration.VertexElements()[6].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float,  declaration.VertexElements()[7].VertexElementFormat);
	EXPECT_EQ(0, declaration.VertexElements()[0].Offset);
	EXPECT_EQ(16, declaration.VertexElements()[1].Offset);
	EXPECT_EQ(16+12, declaration.VertexElements()[2].Offset);
	EXPECT_EQ(16+12+8, declaration.VertexElements()[3].Offset);
	EXPECT_EQ(16+12+8+4, declaration.VertexElements()[4].Offset);
	EXPECT_EQ(16+12+8+4+16, declaration.VertexElements()[5].Offset);
	EXPECT_EQ(16+12+8+4+16+12, declaration.VertexElements()[6].Offset);
	EXPECT_EQ(16+12+8+4+16+12+8, declaration.VertexElements()[7].Offset);
}
