//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "PolygonBatch.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"

namespace Pomdog {
namespace {

// Built-in shaders
#include "Shaders/GLSL.Embedded/LineBatch_VS.inc.h"
#include "Shaders/GLSL.Embedded/LineBatch_PS.inc.h"

struct BuiltinEffectPolygonBatchTrait {
	static std::shared_ptr<EffectPass> Create(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	{
		using Details::ShaderBytecode;
		ShaderBytecode vertexShaderCode = {Builtin_GLSL_LineBatch_VS, std::strlen(Builtin_GLSL_LineBatch_VS)};
		ShaderBytecode pixelShaderCode = {Builtin_GLSL_LineBatch_PS, std::strlen(Builtin_GLSL_LineBatch_PS)};
		return std::make_shared<EffectPass>(graphicsDevice, vertexShaderCode, pixelShaderCode);
	}
};

}// unnamed namespace
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - PolygonBatch::Impl
#endif
//-----------------------------------------------------------------------
class PolygonBatch::Impl {
public:
	static constexpr std::size_t MaxVertexCount = 4096;
	static constexpr std::size_t MinVertexCount = 256;

	struct Vertex {
		// {xyz} = position.xyz
		Vector3 Position;
		
		// {xyzw} = color.rgba
		Vector4 Color;
	};

	std::vector<Vertex> vertices;

private:
	std::shared_ptr<GraphicsContext> graphicsContext;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<EffectPass> effectPass;
	std::shared_ptr<ConstantBufferBinding> constantBuffers;
	std::shared_ptr<InputLayout> inputLayout;

public:
	Impl(std::shared_ptr<GraphicsContext> const& graphicsContext,
		std::shared_ptr<GraphicsDevice> const& graphicsDevice);

	void Begin(Matrix4x4 const& transformMatrix);

	void DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
		Vector4 const& color1, Vector4 const& color2, Vector4 const& color3);
	
	void End();
	
	void Flush();
};
//-----------------------------------------------------------------------
PolygonBatch::Impl::Impl(std::shared_ptr<GraphicsContext> const& graphicsContextIn,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	: graphicsContext(graphicsContextIn)
{
	vertices.reserve(MinVertexCount);
	{
		auto maxVertexCount = MaxVertexCount;
		vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
			maxVertexCount, sizeof(Vertex), BufferUsage::Dynamic);
	}
	{
		effectPass = graphicsDevice->ShaderPool().Create<BuiltinEffectPolygonBatchTrait>(graphicsDevice);
		constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *effectPass);
		inputLayout = std::make_shared<InputLayout>(graphicsDevice, effectPass);
	}
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::Begin(Matrix4x4 const& transformMatrix)
{
	alignas(16) Matrix4x4 transposedMatrix = Matrix4x4::Transpose(transformMatrix);

	auto parameter = constantBuffers->Find("TransformMatrix");
	parameter->SetValue(transposedMatrix);
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::End()
{
	if (vertices.empty()) {
		return;
	}

	Flush();
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::Flush()
{
	POMDOG_ASSERT(!vertices.empty());
	POMDOG_ASSERT(vertices.size() <= MaxVertexCount);
	vertexBuffer->SetData(vertices.data(), static_cast<std::uint32_t>(vertices.size()));
	
	graphicsContext->SetInputLayout(inputLayout);
	graphicsContext->SetVertexBuffer(vertexBuffer);
	graphicsContext->SetEffectPass(effectPass);
	graphicsContext->SetConstantBuffers(constantBuffers);
	graphicsContext->Draw(PrimitiveTopology::TriangleList, static_cast<std::uint32_t>(vertices.size()));
	
	vertices.clear();
}
//-----------------------------------------------------------------------
void PolygonBatch::Impl::DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
	Vector4 const& color1, Vector4 const& color2, Vector4 const& color3)
{
	if (vertices.size() + 3 > MaxVertexCount) {
		Flush();
	}

	POMDOG_ASSERT(vertices.size() + 3 <= Impl::MaxVertexCount);
	vertices.push_back(Vertex{Vector3(point1, 0.0f), color1});
	vertices.push_back(Vertex{Vector3(point2, 0.0f), color2});
	vertices.push_back(Vertex{Vector3(point3, 0.0f), color3});
}
//-----------------------------------------------------------------------
#if defined(POMDOG_COMPILER_CLANG)
#pragma mark - PolygonBatch
#endif
//-----------------------------------------------------------------------
PolygonBatch::PolygonBatch(std::shared_ptr<GraphicsContext> const& graphicsContext,
	std::shared_ptr<GraphicsDevice> const& graphicsDevice)
	: impl(std::make_unique<Impl>(graphicsContext, graphicsDevice))
{}
//-----------------------------------------------------------------------
PolygonBatch::~PolygonBatch() = default;
//-----------------------------------------------------------------------
void PolygonBatch::Begin(Matrix4x4 const& transformMatrixIn)
{
	POMDOG_ASSERT(impl);
	impl->Begin(transformMatrixIn);
}
//-----------------------------------------------------------------------
void PolygonBatch::End()
{
	POMDOG_ASSERT(impl);
	impl->End();
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawCircle(Vector2 const& position, float radius, Color const& color, std::size_t segments)
{
	POMDOG_ASSERT(impl);
	POMDOG_ASSERT(segments >= 3);
	POMDOG_ASSERT(radius >= 0);

	if (radius <= 0) {
		return;
	}

	POMDOG_ASSERT(radius > 0);
	
	POMDOG_ASSERT(segments >= 3);
	Radian<float> centralAngle = MathConstants<float>::TwoPi() / segments;
	Vector2 prevPoint = position + Vector2{radius, 0};
	
	auto colorVector = color.ToVector4();
	
	for (std::size_t i = 0; i < segments; ++i)
	{
		auto rad = centralAngle * (i + 1);
		auto cos = std::cos(rad.value);
		auto sin = std::sin(rad.value);
		auto nextPoint = position + (radius * Vector2{cos, sin});
		impl->DrawTriangle(nextPoint, prevPoint, position, colorVector, colorVector, colorVector);
		prevPoint = nextPoint;
	}
}
//-----------------------------------------------------------------------
//void PolygonBatch::DrawPolyline(std::vector<Vector2> const& points, float thickness, Color const& color)
//{
//	POMDOG_ASSERT(impl);
//	POMDOG_ASSERT(!points.empty());
//	POMDOG_ASSERT(points.size() >= 2);
//	POMDOG_ASSERT(thickness >= 0);
//	
//	if (thickness <= 0) {
//		return;
//	}
//	
//	auto prevPoint = points.front();
//	for (auto iter = std::next(std::begin(points)); iter < std::end(points); ++iter)
//	{
//		auto & origin = *iter;
//		auto tangent = origin - prevPoint;
//		auto normal = Vector2{-tangent.Y, tangent.X};
//		
//		auto p1 = prevPoint + (thickness * normal);
//		auto p2 = prevPoint + (-thickness * normal);
//		
//		Vector2 nextPoint = *iter;
//		auto nextIter = std::next(iter);
//		if (nextIter != std::end(points))
//		{
//			nextPoint = *nextIter;
//			
//			// TODO
//			// ...
//		}
//		else
//		{
//			auto p3 = origin + (thickness * normal);
//			auto p4 = origin + (-thickness * normal);
//			impl->DrawTriangle(p1, p2, p3, color, color, color);
//			impl->DrawTriangle(p3, p4, p1, color, color, color);
//		}
//	}
//}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(Rectangle const& sourceRect, Color const& color)
{
	POMDOG_ASSERT(impl);
	DrawRectangle(sourceRect, color, color, color, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(Rectangle const& sourceRect,
	Color const& color1, Color const& color2, Color const& color3, Color const& color4)
{
	POMDOG_ASSERT(impl);
	
	if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
		return;
	}

	std::array<Vector2, 4> rectVertices = {
		Vector2(sourceRect.Left(), sourceRect.Y),
		Vector2(sourceRect.Left(), sourceRect.Y + sourceRect.Height),
		Vector2(sourceRect.Right(), sourceRect.Y + sourceRect.Height),
		Vector2(sourceRect.Right(), sourceRect.Y),
	};
	
	auto colorVector1 = color1.ToVector4();
	auto colorVector2 = color2.ToVector4();
	auto colorVector3 = color3.ToVector4();
	auto colorVector4 = color4.ToVector4();

	impl->DrawTriangle(rectVertices[0], rectVertices[1], rectVertices[2],
		colorVector1, colorVector4, colorVector3);
	impl->DrawTriangle(rectVertices[2], rectVertices[3], rectVertices[0],
		colorVector3, colorVector2, colorVector1);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(Matrix3x2 const& matrix,
	Rectangle const& sourceRect, Color const& color)
{
	POMDOG_ASSERT(impl);
	DrawRectangle(matrix, sourceRect, color, color, color, color);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawRectangle(Matrix3x2 const& matrix,
	Rectangle const& sourceRect,
	Color const& color1, Color const& color2, Color const& color3, Color const& color4)
{
	POMDOG_ASSERT(impl);
	
	if (sourceRect.Width <= 0 || sourceRect.Height <= 0) {
		return;
	}

	std::array<Vector2, 4> rectVertices = {
		Vector2(sourceRect.Left(), sourceRect.Y),
		Vector2(sourceRect.Left(), sourceRect.Y + sourceRect.Height),
		Vector2(sourceRect.Right(), sourceRect.Y + sourceRect.Height),
		Vector2(sourceRect.Right(), sourceRect.Y),
	};
	
	for (auto & vertex: rectVertices) {
		vertex = Vector2::Transform(vertex, matrix);
	}

	auto colorVector1 = color1.ToVector4();
	auto colorVector2 = color2.ToVector4();
	auto colorVector3 = color3.ToVector4();
	auto colorVector4 = color4.ToVector4();

	impl->DrawTriangle(rectVertices[0], rectVertices[1], rectVertices[2],
		colorVector1, colorVector4, colorVector3);
	impl->DrawTriangle(rectVertices[2], rectVertices[3], rectVertices[0],
		colorVector3, colorVector2, colorVector1);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3, Color const& color)
{
	POMDOG_ASSERT(impl);
	auto colorVector = color.ToVector4();
	impl->DrawTriangle(point1, point2, point3, colorVector, colorVector, colorVector);
}
//-----------------------------------------------------------------------
void PolygonBatch::DrawTriangle(Vector2 const& point1, Vector2 const& point2, Vector2 const& point3,
	Color const& color1, Color const& color2, Color const& color3)
{
	POMDOG_ASSERT(impl);
	auto colorVector1 = color1.ToVector4();
	auto colorVector2 = color2.ToVector4();
	auto colorVector3 = color3.ToVector4();
	impl->DrawTriangle(point1, point2, point3, colorVector1, colorVector2, colorVector3);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
