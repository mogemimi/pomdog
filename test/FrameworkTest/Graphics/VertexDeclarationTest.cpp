//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/VertexDeclaration.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::VertexDeclaration;
using Pomdog::VertexElement;
using Pomdog::VertexElementFormat;

TEST(VertexDeclaration, CopyConstructor)
{
	std::vector<VertexElement> elements {
		{ 0, VertexElementFormat::Float }
	};
	VertexDeclaration declaration(elements);
	ASSERT_EQ(4, sizeof(float));
	EXPECT_EQ(sizeof(float), declaration.StrideBytes());
	
	ASSERT_FALSE(declaration.VertexElements().empty());
	ASSERT_EQ(1, declaration.VertexElements().size());
	auto element = declaration.VertexElements()[0];
	
	EXPECT_EQ(VertexElementFormat::Float, element.VertexElementFormat);
	EXPECT_EQ(0, element.Offset);
}

TEST(VertexDeclaration, Constructor_InitializerList)
{
	VertexDeclaration declaration {
		{ 0, VertexElementFormat::Float4 },
		{ 16, VertexElementFormat::Float3 },
		{ 32, VertexElementFormat::Float2 },
		{ 48, VertexElementFormat::Float }
	};
	ASSERT_EQ(4, sizeof(float));
	EXPECT_EQ(48 + 4, declaration.StrideBytes());
	
	ASSERT_FALSE(declaration.VertexElements().empty());
	ASSERT_EQ(4, declaration.VertexElements().size());
	auto elements = declaration.VertexElements();
	
	EXPECT_EQ(VertexElementFormat::Float4, elements[0].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float3, elements[1].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float2, elements[2].VertexElementFormat);
	EXPECT_EQ(VertexElementFormat::Float, elements[3].VertexElementFormat);
	
	EXPECT_EQ(0, elements[0].Offset);
	EXPECT_EQ(16, elements[1].Offset);
	EXPECT_EQ(32, elements[2].Offset);
	EXPECT_EQ(48, elements[3].Offset);
}
